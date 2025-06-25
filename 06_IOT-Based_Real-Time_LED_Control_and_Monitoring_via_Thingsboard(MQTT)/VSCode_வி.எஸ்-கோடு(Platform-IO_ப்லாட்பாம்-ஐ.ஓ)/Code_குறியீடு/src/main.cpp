#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// Function prototypes
void InitWiFi();
void on_message(const char* topic, byte* payload, unsigned int length);
void reconnect();
String get_led_status();
void set_led_status(int pin, boolean enabled);

#define WIFI_AP "YOUR WIFI SSID"
#define WIFI_PASSWORD "YOUR WIFI PASSWORD"
#define TOKEN "YOUR TOKEN"

#define LED_PIN_0 D0 // GPIO16
#define LED_PIN_1 D1 // GPIO5
#define LED_PIN_2 D2 // GPIO4
#define LED_CONTROL_PIN_0 0
#define LED_CONTROL_PIN_1 1
#define LED_CONTROL_PIN_2 2

char thingsboardServer[] = "demo.thingsboard.io";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
boolean ledStates[3]; // true = ON, false = OFF

unsigned long lastSend = 0; // Timer for telemetry

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN_0, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);

  // Turn all LEDs OFF (active-high: LOW = OFF)
  digitalWrite(LED_PIN_0, LOW);
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);

  delay(50); // Allow time to settle

  // Sync states
  ledStates[0] = digitalRead(LED_PIN_0) == HIGH;
  ledStates[1] = digitalRead(LED_PIN_1) == HIGH;
  ledStates[2] = digitalRead(LED_PIN_2) == HIGH;

  InitWiFi();
  client.setServer(thingsboardServer, 1883);
  client.setCallback(on_message);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Send telemetry every 1 second
  if (millis() - lastSend > 1000) {
    client.publish("v1/devices/me/telemetry", get_led_status().c_str());
    lastSend = millis();
  }
}

void on_message(const char* topic, byte* payload, unsigned int length) {
  Serial.println("On message");

  char json[length + 1];
  strncpy(json, (char*)payload, length);
  json[length] = '\0';

  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  Serial.println(json);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, json);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonObject data = doc.as<JsonObject>();
  String methodName = String(data["method"].as<const char*>());

  if (methodName.equals("getGpioStatus")) {
    String responseTopic = String(topic);
    responseTopic.replace("request", "response");
    client.publish(responseTopic.c_str(), get_led_status().c_str());
  } else if (methodName.equals("setGpioStatus")) {
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

String get_led_status() {
  JsonDocument doc;
  JsonObject data = doc.to<JsonObject>();

  // Send numeric: 1 = ON, 0 = OFF for Simple Gauge
  data[String(LED_CONTROL_PIN_0)] = ledStates[0] ? 1 : 0;
  data[String(LED_CONTROL_PIN_1)] = ledStates[1] ? 1 : 0;
  data[String(LED_CONTROL_PIN_2)] = ledStates[2] ? 1 : 0;

  char payload[256];
  serializeJson(data, payload, sizeof(payload));
  String strPayload = String(payload);
  Serial.print("Get LED status: ");
  Serial.println(strPayload);
  return strPayload;
}

void set_led_status(int pin, boolean enabled) {
  int ledPin = -1;
  int index = -1;

  if (pin == LED_CONTROL_PIN_0) {
    ledPin = LED_PIN_0;
    index = 0;
  } else if (pin == LED_CONTROL_PIN_1) {
    ledPin = LED_PIN_1;
    index = 1;
  } else if (pin == LED_CONTROL_PIN_2) {
    ledPin = LED_PIN_2;
    index = 2;
  }

  if (ledPin != -1 && index != -1) {
    digitalWrite(ledPin, enabled ? HIGH : LOW); // HIGH = ON, LOW = OFF
    ledStates[index] = enabled;
  }
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
      Serial.println("Connected to AP");
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
