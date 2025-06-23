#include <Arduino.h> 
// ESP8266 நிரலாக்கத்திற்கு தேவையான Arduino மைய நிரல் தொகுப்பை சேர்க்கவும் - Include the Arduino core library required for ESP8266 programming

// ESP8266 (NodeMCU) க்கான LED மற்றும் பொத்தான்களின் இணைப்புகள் - Define LED and button pins for ESP8266 (NodeMCU)
const int led1Pin = D0;       // LED 1, D0 (GPIO16) க்கு இணைக்கப்பட்டுள்ளது - LED 1 is connected to D0 (GPIO16) - physical pin D0 on NodeMCU
const int led2Pin = D1;       // LED 2, D1 (GPIO5) க்கு இணைக்கப்பட்டுள்ளது - LED 2 is connected to D1 (GPIO5)
const int led3Pin = D2;       // LED 3, D2 (GPIO4) க்கு இணைக்கப்பட்டுள்ளது - LED 3 is connected to D2 (GPIO4)

const int button1Pin = D5;    // பொத்தான் 1, D5 (GPIO14) க்கு இணைக்கப்பட்டுள்ளது - Button 1 is connected to D5 (GPIO14)
const int button2Pin = D6;    // பொத்தான் 2, D6 (GPIO12) க்கு இணைக்கப்பட்டுள்ளது - Button 2 is connected to D6 (GPIO12)
const int button3Pin = D7;    // பொத்தான் 3, D7 (GPIO13) க்கு இணைக்கப்பட்டுள்ளது - Button 3 is connected to D7 (GPIO13)

void setup() {
  // LED களின் மின்னியல் கொம்கபுளை வெளியீடுகளாக அமைக்கிறது - Initialize LED pins as outputs
  pinMode(led1Pin, OUTPUT);  // LED 1 க்கு D0 ஐ OUTPUT ஆக அமைக்கிறது - Set D0 as OUTPUT for LED 1
  pinMode(led2Pin, OUTPUT);  // LED 2 க்கு D1 ஐ OUTPUT ஆக அமைக்கிறது - Set D1 as OUTPUT for LED 2
  pinMode(led3Pin, OUTPUT);  // LED 3 க்கு D2 ஐ OUTPUT ஆக அமைக்கிறது - Set D2 as OUTPUT for LED 3
  
  // பொத்தான் மின்னியல் கொம்கபுளை உள் Pull-up தடுப்பெதிரிகளுடன் உள்ளீடுகளாக அமைக்கவும் - Initialize button pins as inputs with internal pull-up resistors
  pinMode(button1Pin, INPUT_PULLUP);  // பொத்தான் 1 க்கு D5 ஐ INPUT_PULLUP ஆக அமைக்கிறது - D5 as INPUT with internal pull-up for Button 1
  pinMode(button2Pin, INPUT_PULLUP);  // பொத்தான் 2 க்கு D6 ஐ INPUT_PULLUP ஆக அமைக்கிறது - D6 as INPUT with internal pull-up for Button 2
  pinMode(button3Pin, INPUT_PULLUP);  // பொத்தான் 3 க்கு D7 ஐ INPUT_PULLUP ஆக அமைக்கிறது - D7 as INPUT with internal pull-up for Button 3
}

void loop() {
  // பொத்தாகளின் நிலைகளை கண்டறிகிறது (ஓர் பொத்தான் அழுத்தப்படும்போது அது தாழ்ந்த நிலையாக இருக்கும்) - Read button states (LOW when pressed because of INPUT_PULLUP)
  int button1State = digitalRead(button1Pin);  // பொத்தான் 1 இன் நிலையை கண்டறிகிறது - Read the state of Button 1
  int button2State = digitalRead(button2Pin);  // பொத்தான் 2 இன் நிலையை கண்டறிகிறது - Read the state of Button 2
  int button3State = digitalRead(button3Pin);  // பொத்தான் 3 இன் நிலையை கண்டறிகிறது - Read the state of Button 3
  
  // Button நிலையை அடிப்படையாகக் கொண்டு LED களை கட்டுப்படுத்துகிறது Control LEDs based on button states
  digitalWrite(led1Pin, button1State == LOW ? HIGH : LOW);  // பொத்தான் 1 அழுத்தப்பட்டால் (LOW), LED 1 ஐ இயக்கு - If Button 1 is pressed (LOW), turn ON LED 1
  digitalWrite(led2Pin, button2State == LOW ? HIGH : LOW);  // பொத்தான் 2 அழுத்தப்பட்டால், LED 2 ஐ இயக்கு - If Button 2 is pressed (LOW), turn ON LED 2
  digitalWrite(led3Pin, button3State == LOW ? HIGH : LOW);  // பொத்தான் 3 அழுத்தப்பட்டால், LED 3 ஐ இயக்கு - If Button 3 is pressed (LOW), turn ON LED 3
}
