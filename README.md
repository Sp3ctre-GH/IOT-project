# Motion Sensor System

A simple motion detection system using **ESP8266**, a **Flask server**, and a **web interface**.

---

## 🔧 What the System Does

- **ESP8266**: Detects motion and controls a buzzer.
- **Flask Server**: Stores and processes motion data.
- **Web Interface**: Displays motion status, motion count, last motion time, and a daily activity chart (e.g., 45 motions yesterday, 67 today).

---

## 📦 Requirements

### Hardware
- ESP8266 (e.g., NodeMCU)
- Motion sensor (connected to **D5**)
- Buzzer (connected to **D1**)
- Wi-Fi network

### Software
- Arduino IDE
- Python 3
- Python packages:
  ```bash
  pip install flask requests
  ```
- Any modern web browser

---

## ⚙️ Setup

### 1. ESP8266
- Open `esp8266.ino` in the Arduino IDE.
- Set the following values:
  - `ssid` — your Wi-Fi network name
  - `password` — your Wi-Fi password
  - `pcServerHost` — Flask server IP (e.g., `192.168.1.100`)
  - `ESP8266_IP` — IP address of the ESP8266 (e.g., `192.168.1.105`)
- Upload the code to the ESP8266 board.
- Connect:
  - Motion sensor to **D5**
  - Buzzer to **D1**

### 2. Flask Server
- Make sure Python 3 is installed.
- Install required packages:
  ```bash
  pip install flask requests
  ```
- Place files as follows:
  - `app.py` and `index.html` in the same folder
  - `index.html` inside the `templates/` subfolder
  - `siren.mp3` inside the `static/` folder
- In `app.py`, set the IP of the ESP8266 (e.g., `192.168.1.105`)
- Run the server:
  ```bash
  python app.py
  ```

---

## 🌐 Usage

### Main Page  
**http://192.168.1.100:5000**

- View motion status, count, and last motion time.
- See a daily activity chart.
- Control the buzzer:
  - Test sound
  - Activate siren
  - Reset chart
- The siren plays automatically after **6:00 PM** if motion is detected.

### Backup Page  
**http://192.168.1.105**

- View motion status, count, and last motion time.
- Test the buzzer manually.

---

## 🔔 Buzzer

- Automatically activates after 6:00 PM if motion is detected.
- In test mode, it sounds for 2–3 seconds.

---

## 🔄 Reset

- The **"Reset Chart"** button clears the motion chart.
- Motion count resets automatically at midnight.

---

## 🧰 Troubleshooting

- **Wi-Fi issues?** Double-check `ssid` and `password` in the code.
- **Server down?** Restart `app.py`, and make sure `siren.mp3` is accessible.
- **No motion detected?** Check the sensor connection to pin **D5**.
- **Buzzer not sounding?** Test via the interface and check pin **D1**.
- **Chart not working?** Make sure `motion_history.json` is being updated.

---

## 📝 License

This project is licensed under the **MIT License**. See the `LICENSE` file.
