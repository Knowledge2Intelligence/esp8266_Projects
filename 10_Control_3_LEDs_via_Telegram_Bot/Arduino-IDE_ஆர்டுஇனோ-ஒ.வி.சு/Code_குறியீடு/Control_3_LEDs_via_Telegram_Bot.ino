#include <Arduino.h> // Arduino நூலகம்
#include <ESP8266WiFi.h> // ESP8266 WiFi பயன்பாட்டிற்கான நூலகம்
#include <WiFiClientSecure.h> // HTTPS மற்றும் SSL க்கு
#include <UniversalTelegramBot.h> // Telegram Bot க்கான நூலகம்

// ==== WiFi மற்றும் Telegram Bot அமைப்புகள் ====
const char* ssid = "YOUR_WIFI_SSID";  // உங்கள் WiFi பெயரை இடவும்
const char* password = "YOUR_WIFI_PASSWORD";  // உங்கள் WiFi கடவுச்சொல்லை இடவும்
const char* botToken = "YOUR_TOKEN";  // BotFather இல் பெற்ற Telegram Bot டோக்கன்

WiFiClientSecure client; // HTTPS client உருவாக்கம்
UniversalTelegramBot bot(botToken, client); // Telegram bot ஐ client உடன் இணைத்தல்

// ==== LED அமைப்புகள் ====
const int ledPins[3] = {13, 5, 4};  // D7, D1, D2 ==> GPIO13, GPIO5, GPIO4
int ledBrightness[3] = {255, 255, 255}; // ஒவ்வொரு LED இற்கும் ஆரம்ப பிரகாசம்
bool ledState[3] = {false, false, false}; // LED நிலை (ON=false)

// ==== Polling நிலைமாற்றங்கள் ====
unsigned long lastPoll = 0; // கடைசி Polling நேரம்
const unsigned long botPollingInterval = 1000; // 1 விநாடிக்கு ஒரு முறை Poll பண்ண வேண்டும்

// ==== Telegram Command களை கையாளும் செயல்பாடு ====
void handleCommand(String text, String chat_id) {
  text.toLowerCase(); // எல்லா கட்டளைகளையும் lowercase-ஆக மாற்று

  if (text.startsWith("/led")) { // /led1on, /led2off போன்ற கட்டளைகள்
    int idx = text[4] - '1'; // LED எண் கண்டறி (0, 1, 2)
    if (idx >= 0 && idx < 3) {
      if (text.endsWith("on")) {
        ledState[idx] = true; // ON செய்
        analogWrite(ledPins[idx], ledBrightness[idx]); // பிரகாசத்துடன் ON
        bot.sendMessage(chat_id, "LED " + String(idx + 1) + " turned ON", ""); // பதில் அனுப்பு
      } else if (text.endsWith("off")) {
        ledState[idx] = false; // OFF செய்
        analogWrite(ledPins[idx], 0); // பிரகாசம் 0
        bot.sendMessage(chat_id, "LED " + String(idx + 1) + " turned OFF", ""); // பதில் அனுப்பு
      }
    }
  }

  if (text.startsWith("/fade")) { // /fade1 100 போன்ற கட்டளை
    int idx = text[5] - '1'; // LED எண்
    int value = text.substring(7).toInt(); // பிரகாச மதிப்பு
    if (idx >= 0 && idx < 3 && value >= 0 && value <= 255) {
      ledBrightness[idx] = value; // புதிய பிரகாசம் அமை
      if (ledState[idx]) analogWrite(ledPins[idx], value); // LED ON இருந்தால் அமை
      bot.sendMessage(chat_id, "LED " + String(idx + 1) + " brightness set to " + String(value), "");
    }
  }

  if (text == "/status") { // /status கட்டளை
    String msg = "LED Status:\n";
    for (int i = 0; i < 3; i++) {
      msg += "LED " + String(i + 1) + ": ";
      msg += ledState[i] ? "ON" : "OFF";
      msg += " (Brightness: " + String(ledBrightness[i]) + ")\n";
    }
    bot.sendMessage(chat_id, msg, ""); // நிலையை பதிலாக அனுப்பு
  }
}

// ==== ஆரம்ப செயல்பாடு ====
void setup() {
  Serial.begin(9600); // Serial தொடக்கம்

  // அனைத்து LED களும் OUTPUT ஆக அமைக்கவும்
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
    analogWrite(ledPins[i], 0); // ஆரம்பத்தில் OFF
  }

  // WiFi-இற்கு attempt பண்ணு
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // WiFi இணைந்த பின் IP முகவரி பிரிண்ட்
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  client.setInsecure(); // HTTPS/SSL சான்றிதழ் சோதனையை தவிர்க்கும் (Bot செயல்பட)
}

// ==== மென்மையான எப்போதும் ஓடும் loop ====
void loop() {
  // Telegram Polling நேரம் வந்தால்
  if (millis() - lastPoll > botPollingInterval) {
    int numNewMsgs = bot.getUpdates(bot.last_message_received + 1); // புதிய செய்திகள் பெற
    while (numNewMsgs) {
      for (int i = 0; i < numNewMsgs; i++) {
        String chat_id = bot.messages[i].chat_id; // அனுப்புனர் ID
        String text = bot.messages[i].text; // அனுப்பிய உரை
        handleCommand(text, chat_id); // கட்டளை கையாளு
      }
      numNewMsgs = bot.getUpdates(bot.last_message_received + 1); // மீண்டும் பார்க்கவும்
    }
    lastPoll = millis(); // கடைசி Poll நேரம் புதுப்பி
  }
}
