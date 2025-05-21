Motion Sensor Monitoring System

This project is a motion sensor system using an ESP8266 microcontroller, a Flask server, and a web interface to monitor motion. If the main server is down, the ESP8266 acts as a backup server.

What is it?





ESP8266: Detects motion with a sensor and controls a buzzer.



Flask Server: Stores motion data and serves a web interface.



Web Interface: Displays motion status, count, last motion time, and a daily motion count chart.

Features





Real-time motion detection.



Chart of daily motion counts.



Buzzer control (toggle on/off, test).



Siren plays after 6 PM when motion is detected (if buzzer is enabled).



Reset motion history.



Backup server on ESP8266.

Requirements

Hardware





ESP8266 (e.g., NodeMCU)



Motion sensor (connected to pin D5/14)



Buzzer (connected to pin D1/5)



Wi-Fi network

Software





Arduino IDE for ESP8266



Python 3.x with:





Flask



requests



Web browser (Chrome, Firefox, etc.)



Arduino libraries:





ESP8266WiFi



ESPAsyncWebServer



ArduinoJson



NTPClient



WiFiUdp



EEPROM



ESP8266HTTPClient

Setup





ESP8266 Setup:





Install Arduino IDE.



Add required libraries via Library Manager.



Open esp8266.ino and update:





ssid: Your Wi-Fi network name.



password: Your Wi-Fi password.



pcServerHost: Flask server IP (e.g., 192.168.1.100).



ESP8266_IP: ESP8266 IP (e.g., 192.168.1.105).



Upload the code to the ESP8266.



Connect the motion sensor to D5 and buzzer to D1.



Flask Server Setup:





Install Python 3.x.



Install dependencies:

pip install flask requests



Place app.py and index.html in the same folder, with index.html in a templates folder.



Add siren.mp3 to a static folder.



Update ESP8266_IP in app.py to match the ESP8266 IP.



Run the server:

python app.py



Access the Interface:





Open http://<Flask-Server-IP>:5000 (e.g., http://192.168.1.100:5000).



If the server is down, use http://<ESP8266-IP> (e.g., http://192.168.1.105).

Usage





Main Interface (http://<Flask-Server-IP>:5000):





Shows motion status, count, and last motion time.



Displays a chart of daily motion counts.



Buttons to toggle/test the buzzer, test the siren, or reset the chart.



Siren plays after 6 PM if motion is detected and buzzer is enabled.



Backup Interface (http://<ESP8266-IP>):





Shows motion status, count, and last motion time.



Allows buzzer toggle and test.



Redirects to the main server if it becomes available.



Buzzer:





Activates after 6 PM with motion (if enabled).



Test plays for 2 seconds (ESP8266) or 3 seconds (siren).



Reset:





"Reset Chart" clears motion history.



Motion count resets at midnight.

Troubleshooting





ESP8266 Wi-Fi issues: Check ssid and password.



Server not working: Ensure app.py is running and siren.mp3 is in static.



No motion detected: Verify sensor connection on D5.



Buzzer silent: Check D1 connection and test via the interface.



Chart issues: Ensure motion_history.json is writable.
