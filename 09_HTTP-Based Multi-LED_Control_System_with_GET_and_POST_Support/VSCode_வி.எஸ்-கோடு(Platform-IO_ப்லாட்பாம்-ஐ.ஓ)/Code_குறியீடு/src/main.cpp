#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "YOUR_WIFI_SSID"; // Replace with your WiFi SSID
const char* password = "YOUR_WIFI_PASSWORD"; // Replace with your WiFi password

ESP8266WebServer server(80);

// LED pins for D7, D1, D2
const int ledPins[3] = {13, 5, 4};
int ledBrightness[3] = {255, 255, 255};
bool ledState[3] = {false, false, false};

void handleRoot() {
  server.send(200, "text/plain", "ESP8266 LED Controller API\nAvailable: /led, /pwm, /status");
}

void handleLEDControl() {
  int index = server.arg("index").toInt();
  String state = server.arg("state");

  if (index < 0 || index >= 3) {
    server.send(400, "text/plain", "Invalid LED index");
    return;
  }

  if (state == "on") {
    ledState[index] = true;
    analogWrite(ledPins[index], ledBrightness[index]);
  } else if (state == "off") {
    ledState[index] = false;
    analogWrite(ledPins[index], 0);
  } else {
    server.send(400, "text/plain", "Invalid state (use on/off)");
    return;
  }

  server.send(200, "text/plain", "LED " + String(index) + " set to " + state);
  Serial.printf("Toggle: LED %d to %s\n", index, state.c_str());
}

void handlePWMControl() {
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, server.arg("plain"));

  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  int index = doc["index"];
  int value = doc["value"];

  if (index < 0 || index >= 3 || value < 0 || value > 255) {
    server.send(400, "text/plain", "Invalid index or value");
    return;
  }

  ledBrightness[index] = value;
  if (ledState[index]) {
    analogWrite(ledPins[index], value);
  }

  server.send(200, "text/plain", "PWM set");
  Serial.printf("PWM: LED %d → %d\n", index, value);
}

void handleStatus() {
  JsonDocument doc;

  JsonArray states = doc["states"].to<JsonArray>();
  JsonArray brightness = doc["brightness"].to<JsonArray>();

  for (int i = 0; i < 3; i++) {
    states.add(ledState[i]);
    brightness.add(ledBrightness[i]);
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
  Serial.println("Status sent.");
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    analogWrite(ledPins[i], 0); // All LEDs off initially
  }

  server.on("/", handleRoot);
  server.on("/led", handleLEDControl);                 // GET
  server.on("/pwm", HTTP_POST, handlePWMControl);      // POST
  server.on("/status", handleStatus);                  // GET

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}
