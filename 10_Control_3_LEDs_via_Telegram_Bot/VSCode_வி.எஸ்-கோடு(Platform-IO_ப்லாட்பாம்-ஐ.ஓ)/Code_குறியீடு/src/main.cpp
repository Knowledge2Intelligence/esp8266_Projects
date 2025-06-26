#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// WiFi and Bot
const char* ssid = "YOUR_WIFI_SSID";  // Replace with your WiFi SSID
const char* password = "YOUR_WIFI_PASSWORD";  // Replace with your WiFi Password
// Replace with your Bot Token from BotFather
const char* botToken = "YOUR_TOKEN";

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

// LED pins for D7, D1, D2
const int ledPins[3] = {13, 5, 4};  // GPIO13 (D7), GPIO5 (D1), GPIO4 (D2)
int ledBrightness[3] = {255, 255, 255};
bool ledState[3] = {false, false, false};

unsigned long lastPoll = 0;
const unsigned long botPollingInterval = 1000;

void handleCommand(String text, String chat_id) {
  text.toLowerCase();

  if (text.startsWith("/led")) {
    int idx = text[4] - '1';
    if (idx >= 0 && idx < 3) {
      if (text.endsWith("on")) {
        ledState[idx] = true;
        analogWrite(ledPins[idx], ledBrightness[idx]);
        bot.sendMessage(chat_id, "LED " + String(idx + 1) + " turned ON", "");
      } else if (text.endsWith("off")) {
        ledState[idx] = false;
        analogWrite(ledPins[idx], 0);
        bot.sendMessage(chat_id, "LED " + String(idx + 1) + " turned OFF", "");
      }
    }
  }

  if (text.startsWith("/fade")) {
    int idx = text[5] - '1';
    int value = text.substring(7).toInt();
    if (idx >= 0 && idx < 3 && value >= 0 && value <= 255) {
      ledBrightness[idx] = value;
      if (ledState[idx]) analogWrite(ledPins[idx], value);
      bot.sendMessage(chat_id, "LED " + String(idx + 1) + " brightness set to " + String(value), "");
    }
  }

  if (text == "/status") {
    String msg = "LED Status:\n";
    for (int i = 0; i < 3; i++) {
      msg += "LED " + String(i + 1) + ": ";
      msg += ledState[i] ? "ON" : "OFF";
      msg += " (Brightness: " + String(ledBrightness[i]) + ")\n";
    }
    bot.sendMessage(chat_id, msg, "");
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    analogWrite(ledPins[i], 0);
  }

  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  client.setInsecure();
}

void loop() {
  if (millis() - lastPoll > botPollingInterval) {
    int numNewMsgs = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMsgs) {
      for (int i = 0; i < numNewMsgs; i++) {
        String chat_id = bot.messages[i].chat_id;
        String text = bot.messages[i].text;
        handleCommand(text, chat_id);
      }
      numNewMsgs = bot.getUpdates(bot.last_message_received + 1);
    }
    lastPoll = millis();
  }
}
