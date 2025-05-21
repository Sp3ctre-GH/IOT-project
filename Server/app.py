import json
import logging
from datetime import datetime
from flask import Flask, jsonify, request, render_template
import requests
import threading
import time

logger = logging.getLogger(__name__)
logger.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
file_handler = logging.FileHandler('motion.log', encoding='utf-8')
file_handler.setFormatter(formatter)
logger.addHandler(file_handler)
logging.getLogger('werkzeug').setLevel(logging.CRITICAL)

app = Flask(__name__)

motion_data = {
    'motion': False,
    'motionCount': 0,
    'lastMotionTime': None,
    'previousMotion': None
}
motion_history = []
buzzer_state = {'buzzerEnabled': False}
ESP8266_IP = '192.168.1.105'
last_day = None

try:
    with open('motion_history.json', 'r', encoding='utf-8') as f:
        data = json.load(f)
        motion_history = data.get('history', [])
        buzzer_state['buzzerEnabled'] = data.get('buzzerEnabled', False)
except FileNotFoundError:
    motion_history = []
    buzzer_state['buzzerEnabled'] = False

def save_history_and_buzzer():
    with open('motion_history.json', 'w', encoding='utf-8') as f:
        json.dump({
            'history': motion_history,
            'buzzerEnabled': buzzer_state['buzzerEnabled']
        }, f, indent=4)

def log_daily_motion_count():
    global last_day
    while True:
        now = datetime.now()
        current_day = now.day
        if last_day != current_day and now.hour == 0 and now.minute == 0:
            logger.info(f"Daily motion count: {motion_data['motionCount']}")
            last_day = current_day
        time.sleep(60)

threading.Thread(target=log_daily_motion_count, daemon=True).start()

@app.route('/')
def index():
    return render_template('index.html', data=motion_data, history=motion_history)

@app.route('/status', methods=['POST'])
def update_status():
    data = request.json
    if not isinstance(data, dict):
        return jsonify({'error': 'Invalid data format'}), 400

    motion = data.get('motion')
    motion_count = data.get('motionCount')
    last_motion_time = data.get('lastMotionTime')

    if not isinstance(motion, bool):
        return jsonify({'error': 'Invalid motion value'}), 400
    if not isinstance(motion_count, int) or motion_count < 0:
        return jsonify({'error': 'Invalid motionCount value'}), 400
    if last_motion_time and not isinstance(last_motion_time, str):
        return jsonify({'error': 'Invalid lastMotionTime value'}), 400

    motion_data['motion'] = motion
    motion_data['motionCount'] = motion_count
    motion_data['lastMotionTime'] = last_motion_time or datetime.now().isoformat()

    if motion_data['motion'] and motion_data['motion'] != motion_data['previousMotion']:
        logger.info(f"Motion detected: {motion_count}")

    motion_data['previousMotion'] = motion_data['motion']

    motion_history.append({
        'timestamp': datetime.now().isoformat(),
        'motion': motion,
        'motionCount': motion_count,
        'lastMotionTime': last_motion_time or datetime.now().isoformat()
    })
    save_history_and_buzzer()

    return jsonify({'status': 'success'}), 200

@app.route('/data', methods=['GET'])
def get_data():
    return jsonify(motion_data), 200

@app.route('/history', methods=['GET'])
def get_history():
    return jsonify(motion_history), 200

@app.route('/reset', methods=['POST'])
def reset_data():
    global motion_history
    motion_history = []
    save_history_and_buzzer()
    logger.info('Chart cleared successfully')
    return jsonify({'status': 'reset successful'}), 200

@app.route('/toggle-buzzer', methods=['POST', 'GET'])
def toggle_buzzer():
    try:
        if request.method == 'POST':
            buzzer_state['buzzerEnabled'] = not buzzer_state['buzzerEnabled']
            save_history_and_buzzer()
            logger.info(f"Buzzer turned {'ON' if buzzer_state['buzzerEnabled'] else 'OFF'}")
            response = requests.post(f'http://{ESP8266_IP}/toggle-buzzer', timeout=5)
            response.raise_for_status()
        return jsonify({'buzzerEnabled': buzzer_state['buzzerEnabled']}), 200
    except Exception:
        return jsonify({'error': 'Failed to communicate with ESP8266'}), 500

@app.route('/test-buzzer', methods=['POST'])
def test_buzzer():
    try:
        response = requests.post(f'http://{ESP8266_IP}/test-buzzer', timeout=5)
        response.raise_for_status()
        logger.info("Buzzer test started")
        return response.text, 200
    except Exception:
        return jsonify({'error': 'Failed to communicate with ESP8266'}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=False)