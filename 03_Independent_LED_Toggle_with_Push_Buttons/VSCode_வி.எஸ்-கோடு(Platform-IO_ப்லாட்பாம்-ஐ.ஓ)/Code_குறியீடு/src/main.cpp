#include <Arduino.h> // ஆர்டுயினோ நூலகத்தை இணைக்கவும் / Include the Arduino library

// முள் வரையறைகள் NodeMCU (ESP8266) க்கு / Pin definitions for NodeMCU (ESP8266)
const int buttonPin1 = D5;   // பொத்தான் 1 முள் (D5 = GPIO 5) / Push button 1 pin (D5 = GPIO 5)
const int buttonPin2 = D6;   // பொத்தான் 2 முள் (D6 = GPIO 12) / Push button 2 pin (D6 = GPIO 12)
const int buttonPin3 = D7;   // பொத்தான் 3 முள் (D7 = GPIO 13) / Push button 3 pin (D7 = GPIO 13)
const int ledPin1 = D0;      // LED 1 முள் (D0 = GPIO 16) / LED 1 pin (D0 = GPIO 16)
const int ledPin2 = D1;      // LED 2 முள் (D1 = GPIO 5) / LED 2 pin (D1 = GPIO 5)
const int ledPin3 = D2;      // LED 3 முள் (D2 = GPIO 4) / LED 3 pin (D2 = GPIO 4)

// LED நிலைகளுக்கான மாறிகள் / Variables for LED states
bool ledState1 = false;     // LED 1 நிலை (false = ஆஃப், true = ஆன்) / LED 1 state (false = OFF, true = ON)
bool ledState2 = false;     // LED 2 நிலை / LED 2 state
bool ledState3 = false;     // LED 3 நிலை / LED 3 state

// பொத்தான் நிலைகளுக்கான மாறிகள் / Variables for button states
bool lastButtonState1 = LOW; // பொத்தான் 1 இன் முந்தைய நிலை / Previous state of button 1
bool lastButtonState2 = LOW; // பொத்தான் 2 இன் முந்தைய நிலை / Previous state of button 2
bool lastButtonState3 = LOW; // பொத்தான் 3 இன் முந்தைய நிலை / Previous state of button 3
bool currentButtonState1;    // பொத்தான் 1 இன் தற்போதைய நிலை / Current state of button 1
bool currentButtonState2;    // பொத்தான் 2 இன் தற்போதைய நிலை / Current state of button 2
bool currentButtonState3;    // பொத்தான் 3 இன் தற்போதைய நிலை / Current state of button 3

// புரோகிராம் ஸ்கோப் பிழைகளைத் தவிர்க்க முன்மாதிரி / Function prototype to avoid scope errors
void updateLEDs();

void setup() {
  // முள்களை துவக்கவும் / Initialize pins
  pinMode(buttonPin1, INPUT_PULLUP); // பொத்தான் 1 உள்ளீடு உயர்ந்த இழுப்பு / Button 1 with internal pull-up
  pinMode(buttonPin2, INPUT_PULLUP); // பொத்தான் 2 உள்ளீடு உயர்ந்த இழுப்பு / Button 2 with internal pull-up
  pinMode(buttonPin3, INPUT_PULLUP); // பொத்தான் 3 உள்ளீடு உயர்ந்த இழுப்பு / Button 3 with internal pull-up
  pinMode(ledPin1, OUTPUT);          // LED 1 வெளியீடு / LED 1 as output
  pinMode(ledPin2, OUTPUT);          // LED 2 வெளியீடு / LED 2 as output
  pinMode(ledPin3, OUTPUT);          // LED 3 வெளியீடு / LED 3 as output
  
  // ஆரம்ப LED நிலைகளை அமைக்கவும் / Set initial LED states
  updateLEDs();
}

void loop() {
  // பொத்தான் நிலைகளைப் படிக்கவும் / Read button states
  currentButtonState1 = digitalRead(buttonPin1); // பொத்தான் 1 நிலையைப் படிக்கவும் / Read button 1 state
  currentButtonState2 = digitalRead(buttonPin2); // பொத்தான் 2 நிலையைப் படிக்கவும் / Read button 2 state
  currentButtonState3 = digitalRead(buttonPin3); // பொத்தான் 3 நிலையைப் படிக்கவும் / Read button 3 state
  
  // பொத்தான் 1 அழுத்தத்தைச் சரிபார்க்கவும் (INPUT_PULLUP காரணமாக உயர்ந்தவற்றிலிருந்து குறைவு மாற்றம்) / Check for button 1 press (HIGH to LOW transition due to INPUT_PULLUP)
  if (currentButtonState1 == LOW && lastButtonState1 == HIGH) {
    ledState1 = !ledState1; // LED 1 நிலையை மாற்றவும் / Toggle LED 1 state
    updateLEDs();           // LEDகளை புதுப்பிக்கவும் / Update LEDs
    delay(50);              // டிபவுன்ஸ் தாமதம் / Debounce delay
  }
  
  // பொத்தான் 2 அழுத்தத்தைச் சரிபார்க்கவும் / Check for button 2 press
  if (currentButtonState2 == LOW && lastButtonState2 == HIGH) {
    ledState2 = !ledState2; // LED 2 நிலையை மாற்றவும் / Toggle LED 2 state
    updateLEDs();           // LEDகளை புதுப்பிக்கவும் / Update LEDs
    delay(50);              // டிபவுன்ஸ் தாமதம் / Debounce delay
  }
  
  // பொத்தான் 3 அழுத்தத்தைச் சரிபார்க்கவும் / Check for button 3 press
  if (currentButtonState3 == LOW && lastButtonState3 == HIGH) {
    ledState3 = !ledState3; // LED 3 நிலையை மாற்றவும் / Toggle LED 3 state
    updateLEDs();           // LEDகளை புதுப்பிக்கவும் / Update LEDs
    delay(50);              // டிபவுன்ஸ் தாமதம் / Debounce delay
  }
  
  // கடைசி பொத்தான் நிலைகளை புதுப்பிக்கவும் / Update last button states
  lastButtonState1 = currentButtonState1; // பொத்தான் 1 இன் கடைசி நிலையை புதுப்பிக்கவும் / Update last state of button 1
  lastButtonState2 = currentButtonState2; // பொத்தான் 2 இன் கடைசி நிலையை புதுப்பிக்கவும் / Update last state of button 2
  lastButtonState3 = currentButtonState3; // பொத்தான் 3 இன் கடைசி நிலையை புதுப்பிக்கவும் / Update last state of button 3
}

// அனைத்து LEDகளையும் அவற்றின் நிலைகளின் அடிப்படையில் புதுப்பிக்கும் செயல்பாடு / Function to update all LEDs based on their states
void updateLEDs() {
  digitalWrite(ledPin1, ledState1 ? HIGH : LOW); // LED 1 நிலையை அமைக்கவும் / Set LED 1 state
  digitalWrite(ledPin2, ledState2 ? HIGH : LOW); // LED 2 நிலையை அமைக்கவும் / Set LED 2 state
  digitalWrite(ledPin3, ledState3 ? HIGH : LOW); // LED 3 நிலையை அமைக்கவும் / Set LED 3 state
}