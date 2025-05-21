# Motion Sensor Monitoring System

This project is a motion sensor system using an ESP8266 microcontroller, a Flask server, and a web interface to monitor motion. If the main server is down, the ESP8266 acts as a backup server.

## What is it?

- **ESP8266**: Detects motion with a sensor and controls a buzzer.
- **Flask Server**: Stores motion data and serves a web interface.
- **Web Interface**: Displays motion status, count, last motion time, and a daily motion count chart.

## Features

- Real-time motion detection.
- Chart of daily motion counts.
- Buzzer control (toggle on/off, test).
- Siren plays after 6 PM when motion is detected (if buzzer is enabled).
- Reset motion history.
- Backup server on ESP8266.

## Requirements

### Hardware

- ESP8266 (e.g., NodeMCU)
- Motion sensor (connected to pin D5/14)
- Buzzer (connected to pin D1/5)
- Wi-Fi network

### Software

- Arduino IDE for ESP8266
- Python 3.x with:
  - `Flask`
  - `requests`
- Web browser (Chrome, Firefox, etc.)
- Arduino libraries:
  - `ESP8266WiFi`
  - `ESPAsyncWebServer`
  - `ArduinoJson`
  - `NTPClient`
  - `WiFiUdp`
  - `EEPROM`
  - `ESP8266HTTPClient`

## Setup

1. **ESP8266 Setup**:
   - Install Arduino IDE.
   - Add required libraries via Library Manager.
   - Open `esp8266.ino` and update:
     - `ssid`: Your Wi-Fi network name.
     - `password`: Your Wi-Fi password.
     - `pcServerHost`: Flask server IP (e.g., `192.168.1.100`).
     - `ESP8266_IP`: ESP8266 IP (e.g., `192.168.1.105`).
   - Upload the code to the ESP8266.
   - Connect the motion sensor to D5 and buzzer to D1.

2. **Flask Server Setup**:
   - Install Python 3.x.
   - Install dependencies:
     ```bash
     pip install flask requests
