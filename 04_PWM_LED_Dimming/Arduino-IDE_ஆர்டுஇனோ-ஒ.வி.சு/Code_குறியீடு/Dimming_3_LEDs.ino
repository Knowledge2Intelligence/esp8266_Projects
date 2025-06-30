#include <Arduino.h> 
// Arduino.h நூலகத்தை இணைக்கவும் 
// Include the core Arduino functions library

// ======== LED பின்கள் வரையறை (Pin Definitions for PWM) ========

const int led1 = D5; // D5 = GPIO14
// முதல் LED-க்கு D5 பின்னை பயன்படுத்துகிறோம் (PWM-ஐ ஆதரிக்கிறது)

const int led2 = D6; // D6 = GPIO12
// இரண்டாவது LED-க்கு D6 பின்னை பயன்படுத்துகிறோம்

const int led3 = D7; // D7 = GPIO13
// மூன்றாவது LED-க்கு D7 பின்னை பயன்படுத்துகிறோம்

void setup() {
  // ======== பின்களை OUTPUT ஆக அமைக்கிறோம் ========
  pinMode(led1, OUTPUT); // led1 பின் OUTPUT ஆகும்
  pinMode(led2, OUTPUT); // led2 பின் OUTPUT ஆகும்
  pinMode(led3, OUTPUT); // led3 பின் OUTPUT ஆகும்
}

void loop() {
  // ======== LED-ஐ மெதுவாக பிரகாசமாக்கும் பகுதி ========
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    // பிரகாசம் 0 முதல் 255 வரை 5 படிகள் அளவில் அதிகரிக்கிறது

    analogWrite(led1, brightness); // led1-க்கு பிரகாசத்தை அமை
    analogWrite(led2, brightness); // led2-க்கு பிரகாசத்தை அமை
    analogWrite(led3, brightness); // led3-க்கு பிரகாசத்தை அமை

    delay(30); // ஒவ்வொரு படிக்கும் இடைவேளை 30 மில்லி விநாடி காத்திரு
  }

  // ======== LED-ஐ மெதுவாக மங்கச் செய்யும் பகுதி ========
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    // பிரகாசம் 255 முதல் 0 வரை 5 படிகள் அளவில் குறைக்கிறது

    analogWrite(led1, brightness); // led1-க்கு பிரகாசத்தை அமை
    analogWrite(led2, brightness); // led2-க்கு பிரகாசத்தை அமை
    analogWrite(led3, brightness); // led3-க்கு பிரகாசத்தை அமை

    delay(30); // ஒவ்வொரு படிக்கும் இடைவேளை 30 மில்லி விநாடி காத்திரு
  }
}
