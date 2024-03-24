const int buttonPin = 4;  // Change to the pin where the button is connected
const int ledPin = 13;    // Change to the pin where the LED is connected

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(buttonPin) == HIGH) {
    Serial.println("Button pressed");
    while (digitalRead(buttonPin) == HIGH) {
      // Wait for the button to be released
      delay(50);
    }

    // Notify Python
    Serial.println("DetectHuman");

    // Wait for Python response
    while (!Serial.available()) {
      delay(50);
    }

    String response = Serial.readStringUntil('\n');
    if (response == "HumanDetected") {
      // Turn on the LED
      digitalWrite(ledPin, HIGH);
      delay(5000);  // Wait for 5 seconds
      digitalWrite(ledPin, LOW);  // Turn off the LED
    }
  }
}
