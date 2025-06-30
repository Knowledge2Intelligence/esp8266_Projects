#include <Arduino.h> // Arduino நூலகத்தை சேர்க்கவும் – ESP8266 க்கு தேவையான அடிப்படை நிரல் தொகுப்பு
#include <ESP8266WiFi.h> // WiFi க்கு ESP8266 நூலகம் – WiFi செயல்பாடுகளுக்கு
#include <ESP8266WebServer.h> // ESP8266 வலை சேவையக நூலகம் – Web server உருவாக்க
#include <ESP8266mDNS.h> // mDNS ஆதரவுக்கான நூலகம் (விருப்பதிற்கேற்ப)

const char* ssid = "Airtel_alphaomega"; // உங்கள் WiFi SSID ஐ குறிப்பிடவும்
const char* password = "Al9ha!JPH&0m3gA"; // உங்கள் WiFi கடவுச்சொல்லை குறிப்பிடவும்

ESP8266WebServer server(80); // 80 வலைமனை எண்ணில் வலை சேவையகத்தை துவக்கவும்

const int ledPins[3] = {13, 5, 4}; // LED கள் D7(GPIO13), D1(GPIO5), D2(GPIO4) மீது இணைக்கப்பட்டுள்ளன
int ledBrightness[3] = {0, 0, 0}; // ஒவ்வொரு LED-இற்கும் துவக்க பிரகாசம் 0
bool ledState[3] = {false, false, false}; // LED நிலை OFF (false)


// HTML நிரலை உருவாக்கும் செயல்பாடு
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
  html += "function toggleLED(id){fetch('/toggle?led='+id);}"; // Toggle button நுழைவு
  html += "function setPWM(id,val){fetch('/pwm?led='+id+'&val='+val);}"; // Brightness slider நுழைவு
  html += "function resetAll(){for(let i=0;i<3;i++){fetch('/pwm?led='+i+'&val=0');}}"; // Reset all LED
  html += "</script></body></html>";
  return html; // HTML பக்கம் திருப்பிக் கொடுக்கிறது
}

// முதன்மை பக்கத்திற்கு நிரல் அனுப்பும் செயலி
void handleRoot() {
  server.send(200, "text/html", getHTML()); // HTML ஐ Web பக்கமாக அனுப்புகிறது
}

// Toggle கோரிக்கையை நிர்வகிக்கும் செயலி
void handleToggle() {
  int led = server.arg("led").toInt(); // URL-இல் இருந்து எந்த LED என்று பெற்றல்
  if (led < 0 || led >= 3) {
    server.send(400, "text/plain", "Invalid LED index"); // தவறான முள் எண்ணிக்கை
    return;
  }
  ledState[led] = !ledState[led]; // நிலையை மாற்று
  analogWrite(ledPins[led], ledState[led] ? ledBrightness[led] : 0); // உள்ள பிரகாசம் இருந்தால் ஏற்று
  server.send(200, "text/plain", "Toggled LED"); // பதில் அனுப்பு
}

// Brightness கட்டுப்பாட்டை நிர்வகிக்கும் செயலி
void handlePWM() {
  int led = server.arg("led").toInt(); // எங்கு மாற்ற வேண்டும் என்பதைப் பெறுங்கள்
  int val = server.arg("val").toInt(); // எந்த மதிப்பாக மாற்ற வேண்டும் என்பதைப் பெறுங்கள்
  if (led < 0 || led >= 3 || val < 0 || val > 255) {
    server.send(400, "text/plain", "Invalid input"); // தவறான உள்ளீடு
    return;
  }
  ledBrightness[led] = val; // LED-இன் பிரகாசத்தை புதுப்பிக்கவும்
  if (ledState[led]) {
    analogWrite(ledPins[led], val); // LED இயக்கத்தில் இருந்தால் PWM பிரகாசம் அமைக்கவும்
  }
  server.send(200, "text/plain", "PWM Set"); // பதிலளி
}

// துவக்கம்
void setup() {
  Serial.begin(9600); // Serial தொடக்கம்
  WiFi.begin(ssid, password); // WiFi-க்கு இணைக்கவும்
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // WiFi காத்திருக்கவும்
    Serial.print(".");
  }

  Serial.println("\nConnected! IP Address: ");
  Serial.println(WiFi.localIP()); // இடைமுக முகவரியை அச்சிடுங்கள்

  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT); // ஒவ்வொரு LED முள் வெளியீடாக அமைக்கவும்
    analogWrite(ledPins[i], 0); // ஆரம்பத்தில் OFF செய்
  }

  // URL வழிகாட்டிகள்
  server.on("/", handleRoot); // root URL
  server.on("/toggle", handleToggle); // toggle
  server.on("/pwm", handlePWM); // PWM value மாற்ற

  server.begin(); // சேவையகத்தை துவக்கு
  Serial.println("Web server started");

  // விருப்பமான mDNS – http://ledcontrol.local
  if (MDNS.begin("ledcontrol")) {
    Serial.println("MDNS responder started (http://ledcontrol.local)");
  }
}

// நிரந்தரமாக வாடிக்கையாளர் கோரிக்கைகளை கையாளும்
void loop() {
  server.handleClient(); // வாடிக்கையாளர் வேண்டுகோள்களை நிர்வகிக்கவும்
}
