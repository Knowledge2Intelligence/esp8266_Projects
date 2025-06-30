#include <ArduinoJson.h> // JSON தரவுகளை குறியாக்கம்/பிரேரணம் செய்ய ArduinoJson நூலகத்தை இணைக்கவும்
#include <PubSubClient.h> // MQTT client செயல்பாடுகளுக்கான PubSubClient நூலகத்தை இணைக்கவும்
#include <ESP8266WiFi.h> // ESP8266 இல் WiFi கொள்முதல் செயலையின் நூலகத்தை இணைக்கவும்

// Function முறைகள் முன்பறித்து அறிவிக்கும் பகுதிகள்
void InitWiFi(); // WiFi இணைப்பை தொடங்கும் செயல்பாடு
void on_message(const char* topic, byte* payload, unsigned int length); // MQTT மூலம் வந்த தகவலை நிர்வகிக்கும் callback
void reconnect(); // MQTT மீண்டும் இணைப்பதற்கான செயல்பாடு
String get_led_status(); // LED நிலையை JSON வடிவில் தரும் செயல்பாடு
void set_led_status(int pin, boolean enabled); // LED ON/OFF செயற்கூறு
void set_led_brightness(int pin, int brightness); // PWM மூலம் LED பிரகாசம் மாற்றும் செயற்கூறு

// உங்கள் WiFi மற்றும் ThingsBoard தரவுகளுக்கு மாற்றங்கள்
#define WIFI_AP "YOUR_WIFI_SSID" // உங்கள் WiFi பெயரை இங்கு நிரப்பவும்
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD" // உங்கள் WiFi கடவுச்சொல்லை இங்கு நிரப்பவும்
#define TOKEN "YOUR_THINGSBOARD_TOKEN" // ThingsBoard இல் உங்கள் சாதன டோக்கனை இங்கு நிரப்பவும்

// PWM ஆதரவு உடைய ESP8266 PIN-கள் LED க்காக வரையறை
#define LED_PIN_0 D7 // முதலாம் LED – GPIO13
#define LED_PIN_1 D1 // இரண்டாம் LED – GPIO5
#define LED_PIN_2 D2 // மூன்றாம் LED – GPIO4

// RPC கமாண்டுகளுக்கான LED Controller Index வரையறை
#define LED_CONTROL_PIN_0 0 // Controller Index 0
#define LED_CONTROL_PIN_1 1 // Index 1
#define LED_CONTROL_PIN_2 2 // Index 2

char thingsboardServer[] = "demo.thingsboard.io"; // ThingsBoard MQTT சார்வர் முகவரி

WiFiClient wifiClient; // ESP8266 WiFi client உருவாக்கு
PubSubClient client(wifiClient); // MQTT client உருவாக்கு

int status = WL_IDLE_STATUS; // WiFi நிலை சேமிக்க பயன்படும் மாறி
boolean ledStates[3] = {false, false, false}; // ஒவ்வொரு LED-யின் ON/OFF நிலை சேமிப்புகள்
int ledBrightness[3] = {0, 0, 0}; // ஒவ்வொரு LED-யின் PWM பிரகாச நிலை (0–1023)

// **************** SETUP ****************
void setup() {
  Serial.begin(9600); // Serial Monitor-இல் debug தகவலுக்கு 9600 பவுட் அதிர்தை அமைக்கவும்
  pinMode(LED_PIN_0, OUTPUT); // LED 0 பினை வெளியீடாக அமைக்கவும்
  pinMode(LED_PIN_1, OUTPUT); // LED 1 பினை வெளியீடாக அமைக்கவும்
  pinMode(LED_PIN_2, OUTPUT); // LED 2 பினை வெளியீடாக அமைக்கவும்

  digitalWrite(LED_PIN_0, LOW); // LED 0 தொடக்கத்தில் OFF
  digitalWrite(LED_PIN_1, LOW); // LED 1 தொடக்கத்தில் OFF
  digitalWrite(LED_PIN_2, LOW); // LED 2 தொடக்கத்தில் OFF

  InitWiFi(); // WiFi இணைப்பு தொடங்கு
  client.setServer(thingsboardServer, 1883); // MQTT சார்வரை வைத்துக்கொள்
  client.setCallback(on_message); // MQTT callback செயல்பாட்டை அமை
}

// **************** MAIN LOOP ****************
void loop() {
  if (!client.connected()) { // MQTT இணைக்கப்படவில்லையா என பார்க்கும்
    reconnect(); // இணைக்காமல் இருந்தால் மீண்டும் முயற்சி செய்
  }
  client.loop(); // MQTT குறிப்புகள் & callback செயல்படுத்து
}

// **************** CALLBACK FUNCTIONS ****************
void on_message(const char* topic, byte* payload, unsigned int length) {
  Serial.println("On message"); // தகவல் வந்ததாக Serial-ல் அச்சிடு

  char json[length + 1]; // JSON ஐ ஒரு char வரியாக சேமிக்க
  strncpy(json, (char*)payload, length); // payload ஐ நகலெடு
  json[length] = '\0'; // வரியில் முடிவு குறித்த குறியீட்டுடன் முடிவடையச்செய்

  JsonDocument doc; // JSON நிரல் உருவாக்கு
  DeserializationError error = deserializeJson(doc, json); // JSON பிரிக்க
  if (error) { // பிழை வந்தது எனில்
    Serial.print("deserializeJson() failed: "); Serial.println(error.c_str()); // பிழை செய்தியை அச்சிடு
    return; // செயல்பாட்டை நிறுத்து
  }

  JsonObject data = doc.as<JsonObject>(); // JSON பொருளாக மாற்று
  String methodName = String(data["method"].as<const char*>()); // method பெயரை எடு

  if (methodName == "getGpioStatus") { // status கேட்கப்பட்டால்
    String respTopic = String(topic); // உருவான response topic
    respTopic.replace("request", "response"); // request → response
    client.publish(respTopic.c_str(), get_led_status().c_str()); // JSON நிலை வரக்கேள்
  }
  else if (methodName == "setGpioStatus") { // ON/OFF செயற்கூறு வந்தால்
    JsonObject params = data["params"].as<JsonObject>(); // parameters எடு
    for (JsonPair kv : params) { // ஒவ்வொரு key-value-ஐ loop செய்
      int pin = atoi(kv.key().c_str()); // pin index ஆக மாற்று
      boolean enabled = kv.value().as<boolean>(); // boolean மதிப்பு எடு
      set_led_status(pin, enabled); // LED ON/OFF செய்
    }
    client.publish("v1/devices/me/telemetry", get_led_status().c_str()); // status telemetry சேர்
  }
  else if (methodName == "setBrightness") { // பிரகாசம் மாற்றும்பொழுது
    JsonObject params = data["params"].as<JsonObject>(); 
    for (JsonPair kv : params) {
      int pin = atoi(kv.key().c_str()); 
      int brightness = kv.value().as<int>(); 
      if (ledStates[pin]) { // LED ON என்றால்
        set_led_brightness(pin, brightness); // LED பிரகாசம் மாற்று
      }
    }
    client.publish("v1/devices/me/telemetry", get_led_status().c_str()); // status telemetry சேர்
  }
}

// LED செயல்பாட்டை மாற்ற/function
void set_led_status(int pin, boolean enabled) {
  int ledPin = -1; int index = -1;

  if (pin == LED_CONTROL_PIN_0) { ledPin = LED_PIN_0; index = 0; }
  else if (pin == LED_CONTROL_PIN_1) { ledPin = LED_PIN_1; index = 1; }
  else if (pin == LED_CONTROL_PIN_2) { ledPin = LED_PIN_2; index = 2; }

  if (ledPin != -1) {
    ledStates[index] = enabled; // ON/OFF சேமி
    ledBrightness[index] = enabled ? 1023 : 0; // பிரகாசம் மாற்று
    analogWrite(ledPin, ledBrightness[index]); // PWM மூலம் Set செய்
  }
}

// LED brightness set in PWM
void set_led_brightness(int pin, int brightness) {
  int ledPin = -1; int index = -1;

  if (pin == LED_CONTROL_PIN_0) { ledPin = LED_PIN_0; index = 0; }
  else if (pin == LED_CONTROL_PIN_1) { ledPin = LED_PIN_1; index = 1; }
  else if (pin == LED_CONTROL_PIN_2) { ledPin = LED_PIN_2; index = 2; }

  if (ledPin != -1) {
    brightness = constrain(brightness, 0, 1023); // 0–1023 மத்தியில் கட்டுப்படுத்து
    ledBrightness[index] = brightness; // சேமி
    analogWrite(ledPin, brightness); // PWM மூலம் பிரகாசம் அமை
  }
}

// JSON status தரும் செயல்
String get_led_status() {
  JsonDocument doc; JsonObject data = doc.to<JsonObject>();

  data[String(LED_CONTROL_PIN_0)] = ledStates[0] ? 1 : 0;
  data[String(LED_CONTROL_PIN_1)] = ledStates[1] ? 1 : 0;
  data[String(LED_CONTROL_PIN_2)] = ledStates[2] ? 1 : 0;

  data["brightness_0"] = ledBrightness[0];
  data["brightness_1"] = ledBrightness[1];
  data["brightness_2"] = ledBrightness[2];

  char payload[256]; serializeJson(data, payload, sizeof(payload));
  return String(payload);
}

// WiFi தொடங்கும் செயல்
void InitWiFi() {
  Serial.println("Connecting to AP ..."); // Serial-ல் துவங்குகிறது என அச்சிடு
  WiFi.begin(WIFI_AP, WIFI_PASSWORD); // WiFi இணைக்க
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); } // கட்டுப்பாடு வரை காத்திரு
  Serial.println("Connected to AP"); // இணைக்கப்பட்டது என அச்சிடு
}

// MQTT மீண்டும் இணைப்பு
void reconnect() {
  while (!client.connected()) {
    status = WiFi.status(); if (status != WL_CONNECTED) { InitWiFi(); } // WiFi பிழை இருந்தால் மீண்டும் தொடங்கு

    Serial.print("Connecting to ThingsBoard node ..."); // MQTT தொடங்குகிறது
    if (client.connect("ESP8266 Device", TOKEN, NULL)) { // MQTT ஐ இணைக்க
      Serial.println("[DONE]"); client.subscribe("v1/devices/me/rpc/request/+");
      client.publish("v1/devices/me/telemetry", get_led_status().c_str());
    } else {
      Serial.print("[FAILED] rc="); Serial.print(client.state()); Serial.println(" retrying in 5s");
      delay(5000);
    }
  }
}
