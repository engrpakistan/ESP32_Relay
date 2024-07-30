#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#else
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#endif

// Replace with your network credentials
const char* ssid = "MALIK MAAZ AWAN";
const char* password = "12345678";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Define relay pins
const int relayLightPin = 23;
const int relayFanPin = 22;

// HTML for the web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Engineer from Pakistan</title>
  <style>
 body {
  margin: 0;
  padding: 0;
  text-align: center;
  background-color: #ffffff; /* White background */
  display: flex;
  flex-direction: column;
  align-items: center;
}

#banner {
  width: 100vw; /* 100% of the viewport width */
  height: 300px; /* Fixed height of 300px */
  padding-bottom: 20px;
  object-fit: cover; /* Ensures the image covers the width without distortion */
}

.right-section {
  display: flex;
  flex-direction: column;
  align-items: flex-end;
  margin-right: 20px; /* Adjust as needed */
}

#bulb, #fan {
  width: 46px;
  height: 46px;
  object-fit: cover;
}

h1, h2 {
  color: #FC0A7F; /* Bright pink color */
}

/* Toggle switch styles */
.switch {
  position: relative;
  display: inline-block;
  width: 60px;
  height: 34px;
  margin-top: 10px;
  margin-bottom: 20px;
}

.switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background-color: #ccc;
  transition: .4s;
  border-radius: 34px;
}

.slider:before {
  position: absolute;
  content: "";
  height: 26px;
  width: 26px;
  left: 4px;
  bottom: 4px;
  background-color: white;
  transition: .4s;
  border-radius: 50%;
}

input:checked + .slider {
  background-color: #FC0A7F;
}

input:checked + .slider:before {
  transform: translateX(26px);
}

/* Subscribe button styles */
#sub {
  padding-top: 10px;
  height: 60px;
  width: 150px; /* Adjusted width for better proportions */
  background-color: #FED611; /* Bright yellow background */
  color: white; /* White text */
  border: none; /* Remove border */
  border-radius: 12px; /* Rounded corners */
  font-size: 16px; /* Adjusted font size */
  font-weight: bold; /* Bold text */
  cursor: pointer; /* Pointer cursor on hover */
  transition: background-color 0.3s, color 0.3s; /* Smooth transition for hover effect */
}

#sub:hover {
  background-color: white; /* Change background to white on hover */
  color: #FED611; /* Change text to yellow on hover */
}

 </style>
</head>
<body>
  <h1>ENGINEER FROM PAKISTAN</h1>
  <img src="/SUBSCRIBE" id="banner" alt="Subscribe Banner"> 
  <a href="https://www.youtube.com/channel/UCsQf9zGTax1Qi_v5OIi3tJQ">
    <button id="sub">SUBSCRIBE</button>
  </a>

  <h2>STUDIO LIGHT</h2>
  <img src="/bulb_on" id="bulb" alt="Bulb On">
  <label class="switch">
    <input type="checkbox" id="light-toggle" onchange="toggleRelay('light', this.checked)">
    <span class="slider"></span>
  </label>

  <h2>FAN</h2>
  <img src="/fan" id="fan" alt="Fan">
  <label class="switch">
    <input type="checkbox" id="fan-toggle" onchange="toggleRelay('fan', this.checked)">
    <span class="slider"></span>
  </label>

  <footer>
    <p>&copy; 2024 Engineer from Pakistan</p>
  </footer>

  <script>
    function toggleRelay(relay, state) {
      fetch(`/relay/${relay}/${state ? 'on' : 'off'}`)
        .then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.error('Error:', error));
    }
  </script>
</body>  
</html>)rawliteral";

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Initialize relay pins
  pinMode(relayLightPin, OUTPUT);
  pinMode(relayFanPin, OUTPUT);
  digitalWrite(relayLightPin, HIGH); // Set relays to OFF (assuming active low)
  digitalWrite(relayFanPin, HIGH);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });
  
  // Route for image files
  server.on("/SUBSCRIBE", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/SUBSCRIBE.png", "image/png");
  });
  server.on("/fan", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/fan.png", "image/png");
  });
  server.on("/bulb_on", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(LittleFS, "/bulb_on.png", "image/png");
  });

  // Route for relay control
  server.on("/relay/light/on", HTTP_GET, [](AsyncWebServerRequest* request) {
    digitalWrite(relayLightPin, LOW); // Turn relay on
    request->send(200, "text/plain", "Light relay turned ON");
  });
  server.on("/relay/light/off", HTTP_GET, [](AsyncWebServerRequest* request) {
    digitalWrite(relayLightPin, HIGH); // Turn relay off
    request->send(200, "text/plain", "Light relay turned OFF");
  });
  server.on("/relay/fan/on", HTTP_GET, [](AsyncWebServerRequest* request) {
    digitalWrite(relayFanPin, LOW); // Turn relay on
    request->send(200, "text/plain", "Fan relay turned ON");
  });
  server.on("/relay/fan/off", HTTP_GET, [](AsyncWebServerRequest* request) {
    digitalWrite(relayFanPin, HIGH); // Turn relay off
    request->send(200, "text/plain", "Fan relay turned OFF");
  });

  // Start server
  server.begin();
}

void loop() {
}





