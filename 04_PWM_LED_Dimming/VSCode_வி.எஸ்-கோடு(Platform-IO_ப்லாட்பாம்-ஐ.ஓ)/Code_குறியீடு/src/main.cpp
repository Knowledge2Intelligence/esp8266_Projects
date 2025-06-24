#include <Arduino.h>

// Define PWM pins for the LEDs
// LED-களுக்கான PWM பின்களை வரையறுக்கவும்
const int led1 = D5; // GPIO14
// முதல் LED-க்கு D5 பின்னை அமைக்கவும் (GPIO14)
// Set the first LED pin to D5 (GPIO14)
const int led2 = D6; // GPIO12
// இரண்டாவது LED-க்கு D6 பின்னை அமைக்கவும் (GPIO12)
// Set the second LED pin to D6 (GPIO12)
const int led3 = D7; // GPIO13
// மூன்றாவது LED-க்கு D7 பின்னை அமைக்கவும் (GPIO13)
// Set the third LED pin to D7 (GPIO13)

void setup() {
  // Set LED pins as outputs
  // LED பின்களை வெளியீடாக அமைக்கவும்
  pinMode(led1, OUTPUT);
  // led1 பின்னை வெளியீடாக அமைக்கவும்
  // Set led1 pin as output
  pinMode(led2, OUTPUT);
  // led2 பின்னை வெளியீடாக அமைக்கவும்
  // Set led2 pin as output
  pinMode(led3, OUTPUT);
  // led3 பின்னை வெளியீடாக அமைக்கவும்
  // Set led3 pin as output
}

void loop() {
  // Fade LEDs in
  // LED-களை படிப்படியாக பிரகாசமாக்கவும்
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    // பிரகாசத்தை 0 முதல் 255 வரை 5 படிகளாக உயர்த்தவும்
    // Increase brightness from 0 to 255 in steps of 5
    analogWrite(led1, brightness);
    // led1-க்கு PWM மூலம் பிரகாசத்தை அமைக்கவும்
    // Set brightness for led1 using PWM
    analogWrite(led2, brightness);
    // led2-க்கு PWM மூலம் பிரகாசத்தை அமைக்கவும்
    // Set brightness for led2 using PWM
    analogWrite(led3, brightness);
    // led3-க்கு PWM மூலம் பிரகாசத்தை அமைக்கவும்
    // Set brightness for led3 using PWM
    delay(30); // Adjust for smoother fading
    // மென்மையான மங்குதலுக்காக 30 மில்லி விநாடிகள் காத்திருக்கவும்
    // Wait 30 milliseconds for smoother fading
  }
  
  // Fade LEDs out
  // LED-களை படிப்படியாக மங்கவைக்கவும்
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    // பிரகாசத்தை 255 முதல் 0 வரை 5 படிகளாக குறைக்கவும்
    // Decrease brightness from 255 to 0 in steps of 5
    analogWrite(led1, brightness);
    // led1-க்கு PWM மூலம் பிரகாசத்தை அமைக்கவும்
    // Set brightness for led1 using PWM
    analogWrite(led2, brightness);
    // led2-க்கு PWM மூலம் பிரகாசத்தை அமைக்கவும்
    // Set brightness for led2 using PWM
    analogWrite(led3, brightness);
    // led3-க்கு PWM மூலம் பிரகாசத்தை அமைக்கவும்
    // Set brightness for led3 using PWM
    delay(30); // Adjust for smoother fading
    // மென்மையான மங்குதலுக்காக 30 மில்லி விநாடிகள் காத்திருக்கவும்
    // Wait 30 milliseconds for smoother fading
  }
}