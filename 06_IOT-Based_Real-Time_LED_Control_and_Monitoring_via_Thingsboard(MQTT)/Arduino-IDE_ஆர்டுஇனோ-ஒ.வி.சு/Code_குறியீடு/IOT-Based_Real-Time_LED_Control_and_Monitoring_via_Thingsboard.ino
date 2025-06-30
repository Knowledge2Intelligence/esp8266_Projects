#include <ArduinoJson.h> // JSON தரவுகளை நிர்வகிக்க பயன்படுத்தப்படும் நூலகம் / Library for handling JSON data
#include <PubSubClient.h> // MQTT தொடர்புக்கு பயன்படுத்தப்படும் நூலகம் / Library for MQTT communication
#include <ESP8266WiFi.h> // ESP8266 WiFi தொடர்புக்கான நூலகம் / Library for ESP8266 WiFi

// முன்பே அறிவிக்கப்பட வேண்டிய செயலிகள் / Function declarations
void InitWiFi(); // WiFi தொடங்கும் செயல் / Function to initialize WiFi
void on_message(const char* topic, byte* payload, unsigned int length); // MQTT செய்திகளை வரவேற்கும் செயலி / MQTT message callback
void reconnect(); // MQTT மறுபிரவேசம் செயலி / Function to reconnect MQTT
String get_led_status(); // தற்போதைய LED நிலையை JSON ஆக தரும் செயலி / Get LED status in JSON
void set_led_status(int pin, boolean enabled); // குறிப்பிட்ட LED-ன் நிலையை மாற்றும் செயலி / Set LED ON or OFF

// WiFi மற்றும் Token விவரங்கள் / WiFi & Token credentials
#define WIFI_AP "YOUR_WIFI_SSID" // உங்கள் WiFi பெயரை இங்கு இடவும் / Replace with your WiFi SSID
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD" // உங்கள் கடவுச்சொல்லை இங்கு இடவும் / Replace with your WiFi Password
#define TOKEN "YOUR_THINGSBOARD_TOKEN" // ThingsBoard token இங்கு இடவும் / Put your ThingsBoard token here

// LED க்கான GPIO பின்கள் / LED GPIO pins
#define LED_PIN_0 D7 // LED 0 பின் - GPIO13 / LED pin 0
#define LED_PIN_1 D1 // LED 1 பின் - GPIO5 / LED pin 1
#define LED_PIN_2 D2 // LED 2 பின் - GPIO4 / LED pin 2

// RPC JSON commands இல் பயன்படுத்தப்படும் சுட்டிகள் / LED control indexes in RPC
#define LED_CONTROL_PIN_0 0
#define LED_CONTROL_PIN_1 1
#define LED_CONTROL_PIN_2 2

// ThingsBoard Server முகவரி / ThingsBoard server address
char thingsboardServer[] = "demo.thingsboard.io";

// WiFi மற்றும் MQTT clients / WiFi and MQTT client objects
WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS; // WiFi நிலை / WiFi connection status
boolean ledStates[3]; // மூன்று LEDகளின் நிலைகள் (true = ON, false = OFF) / LED states
unsigned long lastSend = 0; // கடைசி தரவுகளை அனுப்பிய நேரம் / Last telemetry time

void setup() {
  Serial.begin(9600); // Serial தொடங்கவும் / Start Serial communication

  // LED பின்களை OUTPUT ஆக அமைக்கவும் / Set LED pins as output
  pinMode(LED_PIN_0, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);

  // அனைத்து LEDகளையும் OFF செய்யவும் / Turn off all LEDs initially
  digitalWrite(LED_PIN_0, LOW);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  delay(50); // சிறிய தாமதம் / Small delay

  // தற்போதைய LED நிலையை Sync செய்யவும் / Sync initial LED state
  ledStates[0] = digitalRead(LED_PIN_0) == HIGH;
  ledStates[1] = digitalRead(LED_PIN_1) == HIGH;
  ledStates[2] = digitalRead(LED_PIN_2) == HIGH;

  InitWiFi(); // WiFi தொடக்க செயலி / Initialize WiFi
  client.setServer(thingsboardServer, 1883); // MQTT சர்வரை அமைக்கவும் / Set MQTT server
  client.setCallback(on_message); // Callback செயலியை இணைக்கவும் / Attach message handler
}

void loop() {
  if (!client.connected()) {
    reconnect(); // MQTT மறுபிரவேசம் செய்யவும் / Reconnect if disconnected
  }
  client.loop(); // MQTT client லூப் செயல்பாடு / MQTT client background loop

  // ஒவ்வொரு விநாடிக்கும் Telemetry அனுப்பவும் / Send telemetry every 1 second
  if (millis() - lastSend > 1000) {
    client.publish("v1/devices/me/telemetry", get_led_status().c_str()); // Publish LED status
    lastSend = millis(); // நேரத்தை புதுப்பிக்கவும் / Update time
  }
}

// MQTT செய்தி வரும்போது செயல்படும் செயலி / Callback when MQTT message received
void on_message(const char* topic, byte* payload, unsigned int length) {
  Serial.println("On message");

  char json[length + 1];
  strncpy(json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: "); Serial.println(topic);
  Serial.print("Message: "); Serial.println(json);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject data = doc.as<JsonObject>();
  String methodName = String(data["method"].as<const char*>());

  // RPC command: getGpioStatus
  if (methodName.equals("getGpioStatus")) {
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_led_status().c_str());
  }

  // RPC command: setGpioStatus
  else if (methodName.equals("setGpioStatus")) {
    if (data["params"].is<JsonObject>()) {
      JsonObject params = data["params"].as<JsonObject>();
      for (JsonPair kv : params) {
        int pin = atoi(kv.key().c_str());
        boolean enabled = kv.value().as<boolean>();
        set_led_status(pin, enabled);
      }
    }
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_led_status().c_str());
    client.publish("v1/devices/me/telemetry", get_led_status().c_str());
  }
}

// தற்போதைய LED நிலையை JSON ஆக தரும் செயல் / Function to return LED status JSON
String get_led_status() {
  JsonDocument doc;
  JsonObject data = doc.to<JsonObject>();

  data[String(LED_CONTROL_PIN_0)] = ledStates[0] ? 1 : 0;
  data[String(LED_CONTROL_PIN_1)] = ledStates[1] ? 1 : 0;
  data[String(LED_CONTROL_PIN_2)] = ledStates[2] ? 1 : 0;

  char payload[256];
  serializeJson(data, payload, sizeof(payload));
  String strPayload = String(payload);
  Serial.print("Get LED status: "); Serial.println(strPayload);
  return strPayload;
}

// குறிப்பிட்ட LED-க்கு HIGH/LOW அமைக்கும் செயல் / Turn ON/OFF LED based on pin
void set_led_status(int pin, boolean enabled) {
  int ledPin = -1;
  int index = -1;

  if (pin == LED_CONTROL_PIN_0) {
    ledPin = LED_PIN_0; index = 0;
  } else if (pin == LED_CONTROL_PIN_1) {
    ledPin = LED_PIN_1; index = 1;
  } else if (pin == LED_CONTROL_PIN_2) {
    ledPin = LED_PIN_2; index = 2;
  }

  if (ledPin != -1 && index != -1) {
    digitalWrite(ledPin, enabled ? HIGH : LOW);
    ledStates[index] = enabled;
  }
}

// WiFi தொடங்கும் செயலி / WiFi initialization
void InitWiFi() {
  Serial.println("Connecting to AP ...");
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

// MQTT reconnect செயலி / MQTT reconnect function
void reconnect() {
  while (!client.connected()) {
    status = WiFi.status();
    if (status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500); Serial.print(".");
      }
      Serial.println("Connected to AP");
    }

    Serial.print("Connecting to ThingsBoard node ...");
    if (client.connect("ESP8266 Device", TOKEN, NULL)) {
      Serial.println("[DONE]");
      client.subscribe("v1/devices/me/rpc/request/+");
      client.publish("v1/devices/me/telemetry", get_led_status().c_str());
    } else {
      Serial.print("[FAILED] rc="); Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}
