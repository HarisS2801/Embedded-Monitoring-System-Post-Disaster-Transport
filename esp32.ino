#include <WiFi.h>
#include <WebServer.h>

// ===== WIFI =====
const char* ssid = "Dinooshan's A56";
const char* password = "Dino252001";

// ===== WEB SERVER =====
WebServer server(80);

// ===== UART =====
#define RXD2 16   // Arduino Uno TX -> ESP32 GPIO16
#define TXD2 17   // not used now

// ===== ALERT THRESHOLDS =====
#define LEVEL_THRESHOLD 384
#define MAX_DEVIATION 512

// ===== DATA VARIABLES =====
float temperature = 0.0;
float humidity = 0.0;
int sensor1 = 0;
int sensor2 = 0;
String warningMessage = "Waiting for data...";
bool dataAvailable = false;

String buffer = "";

// ===== CALCULATE WARNING =====
void updateWarningMessage() {
  int deviation = abs(sensor1 - sensor2);

  if (sensor1 > LEVEL_THRESHOLD && sensor2 > LEVEL_THRESHOLD) {
    warningMessage = "ALERT - Both sensors above threshold";
  }
  else if (sensor1 <= LEVEL_THRESHOLD && sensor2 <= LEVEL_THRESHOLD) {
    warningMessage = "NO ALERT - Both below threshold";
  }
  else {
    if (deviation > MAX_DEVIATION) {
      warningMessage = "ALERT - Deviation exceeds limit";
    } else {
      warningMessage = "NO ALERT - Deviation acceptable";
    }
  }
}

// ===== ENABLE CORS =====
void sendCORS() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void handleOptions() {
  sendCORS();
  server.send(204);
}

// ===== ROOT =====
void handleRoot() {
  sendCORS();
  server.send(200, "text/plain",
    "ESP32 Disaster Monitoring API is running.\n"
    "Use endpoints:\n"
    "/data\n"
    "/status\n");
}

// ===== STATUS API =====
void handleStatus() {
  sendCORS();

  String json = "{";
  json += "\"wifi\":\"connected\",";
  json += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  json += "\"serialData\":" + String(dataAvailable ? "true" : "false");
  json += "}";

  server.send(200, "application/json", json);
}

// ===== DATA API =====
void handleData() {
  sendCORS();

  String json = "{";
  json += "\"temperature\":" + String(temperature, 1) + ",";
  json += "\"humidity\":" + String(humidity, 1) + ",";
  json += "\"sensor1\":" + String(sensor1) + ",";
  json += "\"sensor2\":" + String(sensor2) + ",";
  json += "\"warning\":\"" + warningMessage + "\",";
  json += "\"timestamp\":" + String(millis());
  json += "}";

  server.send(200, "application/json", json);
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.println();
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/data", HTTP_GET, handleData);

  server.on("/", HTTP_OPTIONS, handleOptions);
  server.on("/status", HTTP_OPTIONS, handleOptions);
  server.on("/data", HTTP_OPTIONS, handleOptions);

  server.begin();
  Serial.println("API server started");
}

// ===== LOOP =====
void loop() {
  server.handleClient();

  while (Serial2.available()) {
    char c = Serial2.read();

    if (c == '\n') {
      buffer.trim();

      int c1 = buffer.indexOf(',');
      int c2 = buffer.indexOf(',', c1 + 1);
      int c3 = buffer.indexOf(',', c2 + 1);

      if (c1 > 0 && c2 > 0 && c3 > 0) {
        temperature = buffer.substring(0, c1).toFloat();
        humidity    = buffer.substring(c1 + 1, c2).toFloat();
        sensor1     = buffer.substring(c2 + 1, c3).toInt();
        sensor2     = buffer.substring(c3 + 1).toInt();

        updateWarningMessage();
        dataAvailable = true;

        Serial.println("Received:");
        Serial.print("Temperature: "); Serial.println(temperature);
        Serial.print("Humidity   : "); Serial.println(humidity);
        Serial.print("Sensor 1   : "); Serial.println(sensor1);
        Serial.print("Sensor 2   : "); Serial.println(sensor2);
        Serial.print("Warning    : "); Serial.println(warningMessage);
        Serial.println("------------------------");
      } else {
        Serial.print("Invalid serial data: ");
        Serial.println(buffer);
      }

      buffer = "";
    } else {
      buffer += c;
    }
  }
}