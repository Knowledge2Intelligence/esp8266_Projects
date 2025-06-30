#include <Arduino.h> // Arduino நிரலாக்கத்தின் முக்கியமான நூலகம்
#include <ESP8266WiFi.h> // ESP8266 WiFi இணைப்பு நூலகம்
#include <ESP8266WebServer.h> // ESP8266 HTTP WebServer க்கு நூலகம்
#include <ArduinoJson.h> // JSON தரவுகள் பரிமாற்றத்திற்கு நூலகம்

// WiFi SSID மற்றும் Password ஐ வரையறுக்கவும்
const char* ssid = "YOUR_WIFI_SSID"; // உங்கள் WiFi பெயர்
const char* password = "YOUR_WIFI_PASSWORD"; // உங்கள் WiFi கடவுச்சொல்

ESP8266WebServer server(80); // 80 வழிமூலம் HTTP சேவையகம் உருவாக்கம்

// LED கள் பயன்படுத்தப்படும் பின்கள் - D7 (GPIO13), D1 (GPIO5), D2 (GPIO4)
const int ledPins[3] = {13, 5, 4}; // 3 LED க்கான GPIO பின்கள்
int ledBrightness[3] = {255, 255, 255}; // ஒவ்வொரு LED இற்கும் பிரகாச நிலை (0 - 255)
bool ledState[3] = {false, false, false}; // ஒவ்வொரு LED இற்கும் ON/OFF நிலை

// "/" URL ஐ அணுகும்போது பதில்
void handleRoot() {
  server.send(200, "text/plain", "ESP8266 LED Controller API\nAvailable: /led, /pwm, /status");
  // உரையாக API endpoint விவரங்களை அனுப்புகிறது
}

// "/led" GET கோரிக்கையை கையாளும் செயல்
void handleLEDControl() {
  int index = server.arg("index").toInt(); // "index" எனும் argument ஐ எடுத்துக்கொள்கிறது
  String state = server.arg("state"); // "state" (on/off) argument

  if (index < 0 || index >= 3) {
    // தவறான LED குறியீடு என்றால்
    server.send(400, "text/plain", "Invalid LED index");
    return;
  }

  if (state == "on") {
    ledState[index] = true; // LED ON ஆக அமை
    analogWrite(ledPins[index], ledBrightness[index]); // அந்நிலையில் பிரகாசம் அமை
  } else if (state == "off") {
    ledState[index] = false; // LED OFF
    analogWrite(ledPins[index], 0); // பிரகாசம் 0
  } else {
    server.send(400, "text/plain", "Invalid state (use on/off)");
    return;
  }

  // JSON பதிலுடன் அனுப்புகிறது
  server.send(200, "text/plain", "LED " + String(index) + " set to " + state);
  Serial.printf("Toggle: LED %d to %s\n", index, state.c_str()); // Debug Serial output
}

// "/pwm" POST கோரிக்கையை கையாளும் செயல்
void handlePWMControl() {
  JsonDocument doc; // JSON வரையறை
  DeserializationError error = deserializeJson(doc, server.arg("plain")); // JSON data deserialize

  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  int index = doc["index"]; // JSON இல் இருந்து LED குறியீடு
  int value = doc["value"]; // JSON இல் இருந்து பிரகாசம்

  if (index < 0 || index >= 3 || value < 0 || value > 255) {
    // தவறான LED குறியீடு அல்லது பிரகாச மதிப்பு
    server.send(400, "text/plain", "Invalid index or value");
    return;
  }

  ledBrightness[index] = value; // புதிய பிரகாசத்தை சேமி
  if (ledState[index]) {
    analogWrite(ledPins[index], value); // LED ON என்றால் புதிய பிரகாசத்தை அமை
  }

  server.send(200, "text/plain", "PWM set");
  Serial.printf("PWM: LED %d → %d\n", index, value); // Debug Serial output
}

// "/status" GET கோரிக்கையை கையாளும் செயல்
void handleStatus() {
  JsonDocument doc; // JSON document

  JsonArray states = doc["states"].to<JsonArray>(); // LED நிலை
  JsonArray brightness = doc["brightness"].to<JsonArray>(); // LED பிரகாசம்

  for (int i = 0; i < 3; i++) {
    states.add(ledState[i]); // ஒவ்வொரு LED இன் நிலையை சேர்
    brightness.add(ledBrightness[i]); // ஒவ்வொரு LED இன் பிரகாசத்தையும் சேர்
  }

  String response;
  serializeJson(doc, response); // JSON string ஆக மாற்று
  server.send(200, "application/json", response); // JSON பதில் அனுப்பு
  Serial.println("Status sent."); // Debug
}

void setup() {
  Serial.begin(9600); // Serial communication துவக்கு
  WiFi.begin(ssid, password); // WiFi-க்கு connect ஆக முயற்சி செய்
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // WiFi இணைப்பு காத்திருப்பு
    Serial.print(".");
  }

  Serial.println("\nConnected! IP: " + WiFi.localIP().toString()); // இணைப்பு வெற்றிகரமாக இருந்தால் IP ஐ காண்பி

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT); // ஒவ்வொரு LED பின்னையும் OUTPUT ஆக அமை
    analogWrite(ledPins[i], 0);  // ஆரம்பத்தில் எல்லா LED களையும் OFF செய்
  }

  server.on("/", handleRoot); // "/" URL இன் ஹேண்ட்லர்
  server.on("/led", handleLEDControl); // "/led" endpoint க்கு ஹேண்ட்லர்
  server.on("/pwm", HTTP_POST, handlePWMControl); // "/pwm" POST endpoint
  server.on("/status", handleStatus); // "/status" endpoint

  server.begin(); // Web server துவக்கு
  Serial.println("Web server started"); // Serial Debug
}

void loop() {
  server.handleClient(); // HTTP client கோரிக்கைகளை கையாளும் மைய செயல்
}
