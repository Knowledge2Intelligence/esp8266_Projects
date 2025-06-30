#include <Arduino.h> 
// Arduino.h நூலகத்தை சேர்க்கவும் 
// Include the Arduino core library

const int ledPin = D1; // D1 = GPIO5
// LED-க்கு D1 (GPIO5) பின்னை அமைக்கவும் 
// Set the LED pin to D1 (GPIO5)

const int potPin = A0; // Analog input pin
// பொட்டென்ஷியோமீட்டருக்காக A0 அனலாக் உள்ளீட்டை பயன்படுத்தவும் 
// Use analog pin A0 for potentiometer

void setup() {
  Serial.begin(115200); 
  // சீரியல் தொடர்பை 115200 baud rate-இல் துவக்கவும் 
  // Start Serial communication at 115200 baud

  pinMode(ledPin, OUTPUT); 
  // ledPin-ஐ OUTPUT ஆக அமைக்கவும் 
  // Set ledPin as output
}

void loop() {
  int potValue = analogRead(potPin); 
  // A0 (potPin) இலிருந்து அனலாக் மதிப்பை படிக்கவும் 
  // Read analog value from potPin (0 to 1023)

  int brightness = map(potValue, 0, 1023, 0, 255); 
  // 0–1023 மதிப்பை 0–255 மதிப்பாக மாற்றவும் 
  // Convert analog value to PWM brightness

  analogWrite(ledPin, brightness); 
  // LED பிரகாசத்தை PWM மூலம் கட்டுப்படுத்தவும் 
  // Control LED brightness using PWM

  Serial.print("Pot Value: "); Serial.print(potValue); 
  Serial.print(" | Brightness: "); Serial.println(brightness);
  // சீரியல் மானிட்டரில் மதிப்புகளை அச்சிடவும் 
  // Print pot value and brightness on Serial Monitor

  delay(100); 
  // ஒவ்வொரு கட்டணத்திற்கும் 100 மில்லி விநாடி தாமதம் விடவும் 
  // Delay for smooth serial printing
}
