#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Function prototypes
void InitWiFi();
void on_message(const char* topic, byte* payload, unsigned int length);
void reconnect();
String get_led_status();
void set_led_status(int pin, boolean enabled);
void set_led_brightness(int pin, int brightness);

#define WIFI_AP "Your WIFI SSID"
#define WIFI_PASSWORD "Your WIFI Password"
#define TOKEN "Your ThingsBoard Device Tokens"

// PWM-capable GPIOs
#define LED_PIN_0 D7  // GPIO13
#define LED_PIN_1 D1  // GPIO5
#define LED_PIN_2 D2  // GPIO4

#define LED_CONTROL_PIN_0 0
#define LED_CONTROL_PIN_1 1
#define LED_CONTROL_PIN_2 2

char thingsboardServer[] = "demo.thingsboard.io";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
boolean ledStates[3] = {false, false, false};      // Track ON/OFF
int ledBrightness[3] = {0, 0, 0};                  // PWM values (0-1023)

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN_0, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);

  // Start with all LEDs OFF
  digitalWrite(LED_PIN_0, LOW);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);

  InitWiFi();
  client.setServer(thingsboardServer, 1883);
  client.setCallback(on_message);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void on_message(const char* topic, byte* payload, unsigned int length) {
  Serial.println("On message");

  char json[length + 1];
  strncpy(json, (char*)payload, length);
  json[length] = '\0';

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject data = doc.as<JsonObject>();
  String methodName = String(data["method"].as<const char*>());

  if (methodName == "getGpioStatus") {
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_led_status().c_str());
  }
  else if (methodName == "setGpioStatus") {
    JsonObject params = data["params"].as<JsonObject>();
    for (JsonPair kv : params) {
      int pin = atoi(kv.key().c_str());
      boolean enabled = kv.value().as<boolean>();
      set_led_status(pin, enabled);
    }
    client.publish("v1/devices/me/telemetry", get_led_status().c_str());
  }
  else if (methodName == "setBrightness") {
    JsonObject params = data["params"].as<JsonObject>();
    for (JsonPair kv : params) {
      int pin = atoi(kv.key().c_str());
      int brightness = kv.value().as<int>();
      if (ledStates[pin]) {
        set_led_brightness(pin, brightness);
      }
    }
    client.publish("v1/devices/me/telemetry", get_led_status().c_str());
  }
}

void set_led_status(int pin, boolean enabled) {
  int ledPin = -1;
  int index = -1;

  if (pin == LED_CONTROL_PIN_0) { ledPin = LED_PIN_0; index = 0; }
  else if (pin == LED_CONTROL_PIN_1) { ledPin = LED_PIN_1; index = 1; }
  else if (pin == LED_CONTROL_PIN_2) { ledPin = LED_PIN_2; index = 2; }

  if (ledPin != -1 && index != -1) {
    ledStates[index] = enabled;
    ledBrightness[index] = enabled ? 1023 : 0;  // Max when ON, 0 when OFF
    analogWrite(ledPin, ledBrightness[index]);
  }
}

void set_led_brightness(int pin, int brightness) {
  int ledPin = -1;
  int index = -1;

  if (pin == LED_CONTROL_PIN_0) { ledPin = LED_PIN_0; index = 0; }
  else if (pin == LED_CONTROL_PIN_1) { ledPin = LED_PIN_1; index = 1; }
  else if (pin == LED_CONTROL_PIN_2) { ledPin = LED_PIN_2; index = 2; }

  if (ledPin != -1 && index != -1) {
    brightness = constrain(brightness, 0, 1023);
    ledBrightness[index] = brightness;
    analogWrite(ledPin, brightness);
  }
}

String get_led_status() {
  JsonDocument doc;
  JsonObject data = doc.to<JsonObject>();

  // For Simple Gauge (ON = 1, OFF = 0)
  data[String(LED_CONTROL_PIN_0)] = ledStates[0] ? 1 : 0;
  data[String(LED_CONTROL_PIN_1)] = ledStates[1] ? 1 : 0;
  data[String(LED_CONTROL_PIN_2)] = ledStates[2] ? 1 : 0;

  // Optional: brightness telemetry
  data["brightness_0"] = ledBrightness[0];
  data["brightness_1"] = ledBrightness[1];
  data["brightness_2"] = ledBrightness[2];

  char payload[256];
  serializeJson(data, payload, sizeof(payload));
  return String(payload);
}

void InitWiFi() {
  Serial.println("Connecting to AP ...");
  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  while (!client.connected()) {
    status = WiFi.status();
    if (status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
    }

    Serial.print("Connecting to ThingsBoard node ...");
    if (client.connect("ESP8266 Device", TOKEN, NULL)) {
      Serial.println("[DONE]");
      client.subscribe("v1/devices/me/rpc/request/+");
      client.publish("v1/devices/me/telemetry", get_led_status().c_str());
    } else {
      Serial.print("[FAILED] rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}
