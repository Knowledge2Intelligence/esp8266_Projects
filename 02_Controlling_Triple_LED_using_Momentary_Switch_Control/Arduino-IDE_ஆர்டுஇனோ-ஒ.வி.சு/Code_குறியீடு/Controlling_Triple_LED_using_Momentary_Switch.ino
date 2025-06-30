// Arduino.h நூலகத்தை சேர்க்க தேவையில்லை (Arduino IDE-ல் default ஆக சேர்க்கப்படும்)
// No need to manually include <Arduino.h> in Arduino IDE

// === LED மற்றும் பொத்தான் முள் வரையறைகள் ===
// Define LED and button pins for NodeMCU

const int led1Pin = D0;  // LED 1 – D0 (GPIO16) – முதலாம் LED D0-க்கு இணைக்கப்பட்டுள்ளது
const int led2Pin = D1;  // LED 2 – D1 (GPIO5)  – இரண்டாம் LED D1-க்கு
const int led3Pin = D2;  // LED 3 – D2 (GPIO4)  – மூன்றாம் LED D2-க்கு

const int button1Pin = D5;  // Button 1 – D5 (GPIO14) – முதலாம் பொத்தான் D5-க்கு
const int button2Pin = D6;  // Button 2 – D6 (GPIO12) – இரண்டாம் பொத்தான் D6-க்கு
const int button3Pin = D7;  // Button 3 – D7 (GPIO13) – மூன்றாம் பொத்தான் D7-க்கு

void setup() {
  // === LED முள் OUT ஆக அமைத்தல் ===
  pinMode(led1Pin, OUTPUT);  // LED 1 பயன்படுத்தும் D0 ஐ OUT ஆக அமைக்கவும்
  pinMode(led2Pin, OUTPUT);  // LED 2 பயன்படுத்தும் D1
  pinMode(led3Pin, OUTPUT);  // LED 3 பயன்படுத்தும் D2

  // === பொத்தான் INPUT_PULLUP ஆக அமைத்தல் ===
  pinMode(button1Pin, INPUT_PULLUP);  // Button 1 பயன்படுத்தும் D5 ஐ INPUT_PULLUP ஆக அமைக்கவும்
  pinMode(button2Pin, INPUT_PULLUP);  // Button 2 பயன்படுத்தும் D6
  pinMode(button3Pin, INPUT_PULLUP);  // Button 3 பயன்படுத்தும் D7
}

void loop() {
  // === பொத்தான்களின் நிலையை வாசிக்கவும் ===
  int button1State = digitalRead(button1Pin);  // Button 1 நிலையை வாசிக்கவும்
  int button2State = digitalRead(button2Pin);  // Button 2
  int button3State = digitalRead(button3Pin);  // Button 3

  // === பொத்தான் அழுத்தம் அடிப்படையில் LEDஐ இயக்கவும் ===
  digitalWrite(led1Pin, button1State == LOW ? HIGH : LOW);  // Button 1 அழுத்தப்பட்டால் LED 1 ON ஆகும், இல்லையெனில் OFF
  digitalWrite(led2Pin, button2State == LOW ? HIGH : LOW);  // Button 2 → LED 2
  digitalWrite(led3Pin, button3State == LOW ? HIGH : LOW);  // Button 3 → LED 3
}
