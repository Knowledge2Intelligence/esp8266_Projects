#include <Arduino.h> // ஆர்டுஇனோ நிரல் தொகுப்பை இணைக்கவும் Include the Arduino library

// மின்னியல் கொம்புகளை வரையறுக்கவும் Define LED pins
const int led1 = D0; // முதல் LED இன் மின்னியல் கொம்பு D0 (GPIO 16) ஆக அமைக்கப்பட்டுள்ளது First LED pin set to D0 (GPIO 16)
const int led2 = D1; // இரண்டாவது LED இன் மின்னியல் கொம்பு D1 (GPIO 5) ஆக அமைக்கப்பட்டுள்ளது Second LED pin set to D1 (GPIO 5)
const int led3 = D2; // மூன்றாவது LED இன் மின்னியல் கொம்பு D2 (GPIO 4) ஆக அமைக்கப்பட்டுள்ளது Third LED pin set to D2 (GPIO 4)

void setup() { // அமைவு செயல்பாடு: துவக்கத்தில் ஒரு முறை இயங்கும் Setup function: runs once at startup
  // LED மின்னியல் கொம்புகளை வெளியீடாக அமைக்கவும்
  pinMode(led1, OUTPUT); // LED1 மின்னியல் கொம்பை வெளியீடாக அமைக்கவும் 
  pinMode(led2, OUTPUT); // LED2 மின்னியல் கொம்பை வெளியீடாக அமைக்கவும் 
  pinMode(led3, OUTPUT); // LED3 மின்னியல் கொம்பை வெளியீடாக அமைக்கவும் 
}

void loop() { // சுழற்சி செயல்பாடு: தொடர்ந்து மீண்டும் மீண்டும் இயங்கும் Loop function: runs repeatedly
  // Turn on LED1 LED1 ஐ ஒளிரச் செய்யவும்
  digitalWrite(led1, HIGH); // led1 ஐ இயங்கி ஒளிர வைக்கிறது Turn led1 on (HIGH)a
  delay(500); // 500 மில்லி வினாடிகள் இடைவேளை Wait for 500 milliseconds
  digitalWrite(led1, LOW); // led1 ஐ அணைக்கிறது Turn led1 off (LOW)s
  delay(500); // 500 மில்லி வினாடிகள் இடைவேளை Wait for 500 milliseconds

  // Turn on LED2 LED2 ஐ ஒளிரச் செய்யவும்
  digitalWrite(led2, HIGH); // led2 ஐ இயங்கி ஒளிர வைக்கிறது Turn led2 on (HIGH)
  delay(500); // 500 மில்லி வினாடிகள் இடைவேளை Wait for 500 milliseconds
  digitalWrite(led2, LOW); // led2 ஐ அணைக்கிறது Turn led2 off (LOW)
  delay(500); // 500 மில்லி வினாடிகள் இடைவேளை Wait for 500 milliseconds

  // LED3 ஐ ஒளிரச் செய்யவும் Turn on LED3
  digitalWrite(led3, HIGH); // led3 ஐ இயங்கி ஒளிர வைக்கிறது Turn led3 on (HIGH)
  delay(500); // 500 மில்லி வினாடிகள் இடைவேளை Wait for 500 milliseconds
  digitalWrite(led3, LOW); // led3 ஐ அணைக்கிறது Turn led3 off (LOW)
  delay(500); // 500 மில்லி வினாடிகள் இடைவேளை Wait for 500 milliseconds
}