#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> // Optional for mDNS

const char* ssid = "Airtel_alphaomega";
const char* password = "Al9ha!JPH&0m3gA";

ESP8266WebServer server(80);

const int ledPins[3] = {13, 5, 4}; // D7, D1, D2
int ledBrightness[3] = {0, 0, 0};
bool ledState[3] = {false, false, false};

// Generate HTML for web page
String getHTML() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>ESP8266 3 LED Control</title>";
  html += "<style>body{font-family:sans-serif;} .led{margin:10px;} input{width:200px;}</style></head><body>";
  html += "<h2>ESP8266 - 3 LED Control</h2>";
  for(int i = 0; i < 3; i++) {
    html += "<div class='led'>LED " + String(i+1) + ": ";
    html += "<button onclick=\"toggleLED(" + String(i) + ")\">Toggle</button> ";
    html += "<input type='range' min='0' max='255' value='" + String(ledBrightness[i]) + "' ";
    html += "oninput='setPWM(" + String(i) + ", this.value)' />";
    html += "<br>State: " + String(ledState[i] ? "ON" : "OFF") + 
            ", Brightness: " + String(ledBrightness[i]) + "</div>";
  }
  html += "<br><button onclick='resetAll()'>Reset All</button>";
  html += "<script>";
  html += "function toggleLED(id){fetch('/toggle?led='+id);}";
  html += "function setPWM(id,val){fetch('/pwm?led='+id+'&val='+val);}";
  html += "function resetAll(){for(let i=0;i<3;i++){fetch('/pwm?led='+i+'&val=0');}}";
  html += "</script></body></html>";
  return html;
}

// Handlers
void handleRoot() {
  server.send(200, "text/html", getHTML());
}

void handleToggle() {
  int led = server.arg("led").toInt();
  if (led < 0 || led >= 3) {
    server.send(400, "text/plain", "Invalid LED index");
    return;
  }
  ledState[led] = !ledState[led];
  analogWrite(ledPins[led], ledState[led] ? ledBrightness[led] : 0);
  server.send(200, "text/plain", "Toggled LED");
}

void handlePWM() {
  int led = server.arg("led").toInt();
  int val = server.arg("val").toInt();
  if (led < 0 || led >= 3 || val < 0 || val > 255) {
    server.send(400, "text/plain", "Invalid input");
    return;
  }
  ledBrightness[led] = val;
  if (ledState[led]) {
    analogWrite(ledPins[led], val);
  }
  server.send(200, "text/plain", "PWM Set");
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected! IP Address: ");
  Serial.println(WiFi.localIP());

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    analogWrite(ledPins[i], 0); // Turn off initially
  }

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/pwm", handlePWM);
  server.begin();
  Serial.println("Web server started");

  // Optional: mDNS
  if (MDNS.begin("ledcontrol")) {
    Serial.println("MDNS responder started (http://ledcontrol.local)");
  }
}

void loop() {
  server.handleClient();
}
