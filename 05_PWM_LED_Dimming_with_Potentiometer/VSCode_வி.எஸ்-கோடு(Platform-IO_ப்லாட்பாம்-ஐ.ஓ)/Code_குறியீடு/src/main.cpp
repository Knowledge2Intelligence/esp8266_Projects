#include <Arduino.h>
// Arduino.h நூலகத்தை சேர்க்கவும்
// Include the Arduino.h library

const int ledPin = D1; // GPIO5
// LED-க்கு D1 பின்னை அமைக்கவும் (GPIO5)
// Set the LED pin to D1 (GPIO5)
const int potPin = A0; // Analog input pin
// பொட்டென்ஷியோமீட்டருக்கு A0 பின்னை அமைக்கவும் (அனலாக் உள்ளீடு)
// Set the potentiometer pin to A0 (analog input)

void setup() {
  Serial.begin(115200); // Start Serial communication
  // சீரியல் தொடர்பைத் தொடங்கவும்
  // Start Serial communication
  pinMode(ledPin, OUTPUT);
  // ledPin பின்னை வெளியீடாக அமைக்கவும்
  // Set ledPin as output
}

void loop() {
  int potValue = analogRead(potPin);
  // potPin இலிருந்து அனலாக் மதிப்பைப் படிக்கவும்
  // Read the analog value from potPin
  int brightness = map(potValue, 0, 1023, 0, 255);
  // potValue ஐ 0–1023 இலிருந்து 0–255 ஆக மாற்றவும்
  // Map potValue from 0–1023 to 0–255
  analogWrite(ledPin, brightness);
  // ledPin-க்கு PWM மூலம் பிரகாசத்தை அமைக்கவும்
  // Set brightness for ledPin using PWM
  Serial.print("Pot Value: "); Serial.print(potValue); Serial.print(" | Brightness: "); Serial.println(brightness);
  // பொட்டென்ஷியோமீட்டர் மதிப்பு மற்றும் பிரகா�சத்தை சீரியல் மானிட்டரில் அச்சிடவும்
  // Print potentiometer value and brightness to Serial Monitor
  delay(100); // Delay for readable Serial output
  // படிக்கக்கூடிய சீரியல் வெளியீட்டிற்கு 100 மில்லி விநாடிகள் தாமதம்
  // Delay 100 milliseconds for readable Serial output
}