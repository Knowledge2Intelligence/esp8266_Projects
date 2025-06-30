// === முள் வரையறைகள் (Pin Definitions) ===

const int buttonPin1 = D5;  // பொத்தான் 1 (D5 = GPIO14) – NodeMCU-வில் D5
const int buttonPin2 = D6;  // பொத்தான் 2 (D6 = GPIO12)
const int buttonPin3 = D7;  // பொத்தான் 3 (D7 = GPIO13)

const int ledPin1 = D0;     // LED 1 (D0 = GPIO16)
const int ledPin2 = D1;     // LED 2 (D1 = GPIO5)
const int ledPin3 = D2;     // LED 3 (D2 = GPIO4)

// === LED நிலைகள் (LED states) ===
bool ledState1 = false;  // LED 1 நிலை – false என்றால் OFF, true என்றால் ON
bool ledState2 = false;  // LED 2 நிலை
bool ledState3 = false;  // LED 3 நிலை

// === பொத்தான் நிலைகள் (Button states) ===
bool lastButtonState1 = LOW;  // முந்தைய நிலை – Button 1
bool lastButtonState2 = LOW;  // Button 2
bool lastButtonState3 = LOW;  // Button 3

bool currentButtonState1;     // தற்போதைய நிலை – Button 1
bool currentButtonState2;     // Button 2
bool currentButtonState3;     // Button 3

// === LED நிலைகளைப் புதுப்பிக்கும் செயல்பாடு (Prototype) ===
void updateLEDs();

void setup() {
  // === பொத்தான் பின்கள் உள்ளீடாக Pull-up உடன் அமைக்கவும் ===
  pinMode(buttonPin1, INPUT_PULLUP);  // Button 1 – INPUT_PULLUP
  pinMode(buttonPin2, INPUT_PULLUP);  // Button 2 – INPUT_PULLUP
  pinMode(buttonPin3, INPUT_PULLUP);  // Button 3 – INPUT_PULLUP

  // === LED பின்களை வெளியீடாக அமைக்கவும் ===
  pinMode(ledPin1, OUTPUT);  // LED 1 – OUTPUT
  pinMode(ledPin2, OUTPUT);  // LED 2 – OUTPUT
  pinMode(ledPin3, OUTPUT);  // LED 3 – OUTPUT

  // ஆரம்ப நிலையில் LED நிலைகளை REFRESH செய்யவும்
  updateLEDs();
}

void loop() {
  // === பொத்தான் நிலைகளை வாசிக்கவும் ===
  currentButtonState1 = digitalRead(buttonPin1);
  currentButtonState2 = digitalRead(buttonPin2);
  currentButtonState3 = digitalRead(buttonPin3);

  // === Button 1 அழுத்தம் HIGH → LOW என்றால் Toggle செய்யவும் ===
  if (currentButtonState1 == LOW && lastButtonState1 == HIGH) {
    ledState1 = !ledState1;   // LED 1 நிலையை மாற்றவும்
    updateLEDs();             // LED ஐ புதுப்பிக்கவும்
    delay(50);                // Debounce தாமதம்
  }

  // === Button 2 toggle ===
  if (currentButtonState2 == LOW && lastButtonState2 == HIGH) {
    ledState2 = !ledState2;   // LED 2 நிலையை மாற்றவும்
    updateLEDs();             // Update LEDs
    delay(50);                // Debounce delay
  }

  // === Button 3 toggle ===
  if (currentButtonState3 == LOW && lastButtonState3 == HIGH) {
    ledState3 = !ledState3;   // LED 3 நிலையை மாற்றவும்
    updateLEDs();             // Update LEDs
    delay(50);                // Debounce delay
  }

  // === கடந்த நிலைகளை புதுப்பிக்கவும் (Next loop காக) ===
  lastButtonState1 = currentButtonState1;
  lastButtonState2 = currentButtonState2;
  lastButtonState3 = currentButtonState3;
}

// === அனைத்து LED களையும் அவற்றின் நிலையின் அடிப்படையில் கட்டுப்படுத்து ===
void updateLEDs() {
  digitalWrite(ledPin1, ledState1 ? HIGH : LOW);  // LED 1 ON / OFF
  digitalWrite(ledPin2, ledState2 ? HIGH : LOW);  // LED 2 ON / OFF
  digitalWrite(ledPin3, ledState3 ? HIGH : LOW);  // LED 3 ON / OFF
}
