#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "MALIK MAAZ AWAN";
const char* password = "12345678";

WebServer server(80);

const int relay1Pin = 23;
const int relay2Pin = 22;

void handleRoot() {
  String html = "<html>\
                  <head>\
                    <title>ESP32 Relay Control</title>\
                    <style>\
                      button { padding: 20px; font-size: 20px; }\
                    </style>\
                  </head>\
                  <body>\
                    <h1>ESP32 Relay Control</h1>\
                    <button onclick=\"location.href='/relay1/on'\">Relay 1 ON</button>\
                    <button onclick=\"location.href='/relay1/off'\">Relay 1 OFF</button><br><br>\
                    <button onclick=\"location.href='/relay2/on'\">Relay 2 ON</button>\
                    <button onclick=\"location.href='/relay2/off'\">Relay 2 OFF</button>\
                  </body>\
                </html>";
  server.send(200, "text/html", html);
}

void handleRelay1On() {
  digitalWrite(relay1Pin, LOW); // LOW to turn the relay ON
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRelay1Off() {
  digitalWrite(relay1Pin, HIGH); // HIGH to turn the relay OFF
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRelay2On() {
  digitalWrite(relay2Pin, LOW); // LOW to turn the relay ON
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleRelay2Off() {
  digitalWrite(relay2Pin, HIGH); // HIGH to turn the relay OFF
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  digitalWrite(relay1Pin, HIGH); // Ensure the relay is OFF initially
  digitalWrite(relay2Pin, HIGH); // Ensure the relay is OFF initially

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");

  server.on("/", handleRoot);
  server.on("/relay1/on", handleRelay1On);
  server.on("/relay1/off", handleRelay1Off);
  server.on("/relay2/on", handleRelay2On);
  server.on("/relay2/off", handleRelay2Off);
  Serial.println("IP Address: " + WiFi.localIP().toString());
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
