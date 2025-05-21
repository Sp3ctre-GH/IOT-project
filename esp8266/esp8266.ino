#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>

const int motionSensorPin = 14;
const int buzzerPin = 5;
bool motionDetected = false;
bool buzzerEnabled = true;
bool wifiConnected = false;
unsigned long lastMotionChange = 0;
int motionCount = 0;
unsigned long lastBuzzerTest = 0;
bool isBuzzerTesting = false;
String lastMotionTime = "Немає даних";
int lastDay = -1;
bool lastBuzzerState = false;
bool prevMotionDetected = false;
int prevMotionCount = -1;
bool prevCurrentState = false;
unsigned long startTime = 0;
const long initDelay = 3000;

const char* ssid = "HomeNet_67";
const char* password = "12345678";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 10800, 60000);

const char* pcServerHost = "192.168.1.100";
const int pcServerPort = 5000;

AsyncWebServer server(80);

const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html lang="uk">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Резервний сервер</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f0f0f0;
      text-align: center;
      margin-top: 50px;
    }
    h1 {
      color: #333;
    }
    p {
      font-size: 18px;
      margin: 10px 0;
    }
    button {
      padding: 10px 20px;
      margin: 5px;
      font-size: 16px;
      background-color: #4CAF50;
      color: white;
      border: none;
      cursor: pointer;
    }
    button:hover {
      background-color: #45a049;
    }
  </style>
</head>
<body>
  <h1>Резервний сервер</h1>
  <p id="serverStatus" style="display:none;">Основний сервер недоступний, використовується резервний</p>
  <p>Статус: <span id="motionStatus">Завантаження...</span></p>
  <p>Лічильник спрацьовувань: <span id="motionCount">Завантаження...</span></p>
  <p>Останнє спрацьовування: <span id="lastMotionTime">Завантаження...</span></p>
  <button onclick="toggleBuzzer()">Вимкнути зумер</button>
  <button onclick="testBuzzer()">Перевірити звук</button>
  <script>
    function checkMainServer() {
      fetch('/check-main-server')
        .then(response => response.json())
        .then(data => {
          if (data.redirect) window.location.href = data.redirect;
          else {
            document.getElementById('serverStatus').style.display = 'block';
            updateStatus();
            setInterval(updateStatus, 1000);
          }
        })
        .catch(() => {
          document.getElementById('serverStatus').style.display = 'block';
          updateStatus();
          setInterval(updateStatus, 1000);
        });
    }
    function updateStatus() {
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById('motionStatus').textContent = data.motion ? 'Виявлено рух' : 'Немає руху';
          document.getElementById('motionCount').textContent = data.motionCount;
          document.getElementById('lastMotionTime').textContent = data.lastMotionTime || 'Немає даних';
        })
        .catch(() => {
          document.getElementById('motionStatus').textContent = 'Помилка';
          document.getElementById('motionCount').textContent = 'Помилка';
          document.getElementById('lastMotionTime').textContent = 'Помилка';
        });
    }
    function toggleBuzzer() {
      fetch('/toggle-buzzer', { method: 'POST' })
        .then(response => response.json())
        .then(data => {
          document.querySelector('button[onclick="toggleBuzzer()"]').textContent = data.buzzerEnabled ? 'Вимкнути зумер' : 'Увімкнути зумер';
        });
    }
    function testBuzzer() {
      fetch('/test-buzzer', { method: 'POST' });
    }
    checkMainServer();
  </script>
</body>
</html>
)rawliteral";

void connectToWiFi() {
  if (WiFi.status() != WL_CONNECTED && !wifiConnected) {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) delay(500);
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to Wi-Fi");
      Serial.println(WiFi.localIP());
      wifiConnected = true;
    } else {
      Serial.println("Failed to connect to Wi-Fi");
    }
  }
}

bool isMainServerAvailable() {
  WiFiClient client;
  if (!client.connect(pcServerHost, pcServerPort)) return false;
  client.print(String("GET /status HTTP/1.1\r\n") + "Host: " + pcServerHost + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0 && millis() - timeout < 2000) {}
  while (client.available()) {
    if (client.readStringUntil('\r').indexOf("200 OK") != -1) {
      client.stop();
      return true;
    }
  }
  client.stop();
  return false;
}

String formatTime() {
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  char formattedTime[20];
  sprintf(formattedTime, "%04d-%02d-%02d %02d:%02d", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min);
  return String(formattedTime);
}

bool isWorkingHours() {
  timeClient.update();
  return timeClient.getHours() >= 10 && timeClient.getHours() < 18;
}

void initEEPROM() {
  EEPROM.begin(512);
  motionCount = EEPROM.read(0);
  if (motionCount < 0 || motionCount > 1000) motionCount = 0;
  String storedTime = "";
  for (int i = 1; i <= 19; i++) {
    char c = (char)EEPROM.read(i);
    if (c == 0 || i == 19) break;
    storedTime += c;
  }
  lastMotionTime = storedTime.length() > 0 ? storedTime : "Немає даних";
  prevMotionCount = motionCount;
  delay(100);
}

void saveEEPROM() {
  EEPROM.write(0, motionCount);
  for (int i = 1; i <= 19; i++) {
    if (i - 1 < lastMotionTime.length()) EEPROM.write(i, lastMotionTime[i - 1]);
    else EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void sendDataToPC(bool motion, int count, String time, bool showSuccessMessage) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Cannot send data: Wi-Fi not connected");
    return;
  }

  HTTPClient http;
  WiFiClient client;
  String url = String("http://") + pcServerHost + ":" + pcServerPort + "/status";
  http.begin(client, url);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> jsonDoc;
  jsonDoc["motion"] = motion;
  jsonDoc["motionCount"] = count;
  jsonDoc["lastMotionTime"] = time;
  String jsonData;
  serializeJson(jsonDoc, jsonData);

  int httpCode = http.POST(jsonData);
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK && showSuccessMessage) {
      Serial.println("Data sent to PC server successfully");
    }
  } else {
    Serial.println("Error on HTTP request to PC server");
  }
  http.end();
}

void setup() {
  pinMode(motionSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, HIGH); 
  Serial.begin(115200);
  delay(1000);
  initEEPROM();
  connectToWiFi();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Setup failed: No Wi-Fi connection");
    return;
  }
  timeClient.begin();
  timeClient.setTimeOffset(10800);
  timeClient.update();
  Serial.println("Initial time: " + formatTime());
  startTime = millis();
  Serial.println("Waiting for sensor stabilization (3 seconds)...");
  while (millis() - startTime < initDelay) delay(100);
  Serial.println("Sensor stabilization complete");
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", htmlPage);
  });
  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["motion"] = motionDetected;
    jsonDoc["motionCount"] = motionCount;
    jsonDoc["lastMotionTime"] = lastMotionTime;
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    request->send(200, "application/json", jsonResponse);
  });
  server.on("/toggle-buzzer", HTTP_POST, [](AsyncWebServerRequest *request) {
    buzzerEnabled = !buzzerEnabled;
    bool currentBuzzerState = isBuzzerTesting || (motionDetected && buzzerEnabled && !isWorkingHours());
    digitalWrite(buzzerPin, !currentBuzzerState ? HIGH : LOW);
    Serial.println("Buzzer toggled, new state: " + String(buzzerEnabled ? "Enabled" : "Disabled"));
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["buzzerEnabled"] = buzzerEnabled;
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    request->send(200, "application/json", jsonResponse);
  });
  server.on("/test-buzzer", HTTP_POST, [](AsyncWebServerRequest *request) {
    lastBuzzerTest = millis();
    isBuzzerTesting = true;
    digitalWrite(buzzerPin, LOW); 
    Serial.println("Buzzer test started at " + formatTime());
    request->send(200, "text/plain", "Buzzer test started");
  });
  server.on("/check-main-server", HTTP_GET, [](AsyncWebServerRequest *request) {
    StaticJsonDocument<200> jsonDoc;
    jsonDoc["redirect"] = isMainServerAvailable() ? String("http://") + pcServerHost + ":" + pcServerPort : "";
    String jsonResponse;
    serializeJson(jsonDoc, jsonResponse);
    request->send(200, "application/json", jsonResponse);
  });
  Serial.println("Starting server...");
  server.begin();
  Serial.println(String("Server started on ") + WiFi.localIP().toString());
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
    return;
  }

  timeClient.update();
  int currentDay = timeClient.getDay();
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  
  if (currentHour == 0 && currentMinute == 0 && lastDay != currentDay) {
    motionCount = 0;
    lastMotionTime = "Немає даних";
    lastDay = currentDay;
    saveEEPROM();
    Serial.println("Motion count and lastMotionTime reset at midnight");
  }

  bool currentState = digitalRead(motionSensorPin) == HIGH;
  if (currentState != prevCurrentState) {
    Serial.println("Sensor state changed to: " + String(currentState));
    prevCurrentState = currentState;
  }

  bool stableState = currentState;
  for (int i = 0; i < 3; i++) {
    delay(50);
    if (digitalRead(motionSensorPin) == HIGH != currentState) {
      stableState = false;
      break;
    }
  }

  if (stableState != motionDetected && millis() - lastMotionChange > 500 && millis() - startTime > initDelay) {
    motionDetected = stableState;
    lastMotionChange = millis();
    if (motionDetected != prevMotionDetected) {
      if (motionDetected) {
        motionCount++;
        lastMotionTime = formatTime();
        saveEEPROM();
        Serial.print("Sensor triggered at ");
        Serial.print(lastMotionTime);
        Serial.print(", motionCount: ");
        Serial.println(motionCount);
      } else {
        Serial.println("Sensor stopped");
      }
      sendDataToPC(motionDetected, motionCount, lastMotionTime, true);
      prevMotionDetected = motionDetected;
    }
  }

  bool currentBuzzerState = isBuzzerTesting || (motionDetected && buzzerEnabled && !isWorkingHours());
  if (currentBuzzerState != lastBuzzerState) {
    digitalWrite(buzzerPin, !currentBuzzerState ? HIGH : LOW); 
    Serial.println(currentBuzzerState ? "Buzzer ON" : "Buzzer OFF");
    lastBuzzerState = currentBuzzerState;
  }

  if (isBuzzerTesting && millis() - lastBuzzerTest >= 2000) {
    digitalWrite(buzzerPin, HIGH); 
    isBuzzerTesting = false;
    Serial.println("Buzzer test ended");
  }

  delay(300);
}