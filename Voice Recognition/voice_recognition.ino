// Define the pin for the button
/*const int buttonPin = 9;
const int ledPin3 = 13; // Replace with the pin connected to your LED
const int ledPin2 = 12;
const int ledPin1 = 11;
const int ledPin0 = 10;
bool buttonPressed = false;

void setup() {
  // Initialize the button pin as an input
  pinMode(buttonPin, INPUT_PULLUP);
  // Initialize the LED pin as an output
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin0, OUTPUT);
  // Start serial communication at 9600 baud
  Serial.begin(9600);
}

void loop() {
  // Read the state of the button
  int buttonState = digitalRead(buttonPin);

  // If the button is pressed (LOW), send a signal
  if (buttonState == LOW) {
    if (!buttonPressed) {
      Serial.println("ButtonPressed");
      buttonPressed = true; // Set a flag to avoid multiple presses
      delay(500); // Debounce delay (adjust as needed)
    }
  } else {
    buttonPressed = false; // Reset the flag when the button is released
  }

  // Check for a voice command
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    // Check if the recognized command contains any of the specified substrings for floors
    if (command.indexOf("first") != -1 || command.indexOf("1") != -1 || command.indexOf("1st") != -1 || command.indexOf("one") != -1) {
      Serial.println("Recognized command: " + command);
      digitalWrite(ledPin1, HIGH); // Turn on the LED
      delay(10000); // Keep the LED on for 1 second (adjust as needed)
      digitalWrite(ledPin1, LOW); // Turn off the LED
    }
    else if (command.indexOf("2nd") != -1 || command.indexOf("2nd floor") != -1 || command.indexOf("second floor") != -1 || command.indexOf("2") != -1) {
      Serial.println("Recognized command: " + command);
      digitalWrite(ledPin2, HIGH); // Turn on the LED
      delay(10000); // Keep the LED on for 1 second (adjust as needed)
      digitalWrite(ledPin2, LOW); // Turn off the LED
    }
    else if (command.indexOf("3rd floor") != -1 || command.indexOf("3 floor") != -1 || command.indexOf("3rd") != -1|| command.indexOf("third") != -1) {
      Serial.println("Recognized command: " + command);
      digitalWrite(ledPin3, HIGH); // Turn on the LED
      delay(10000); // Keep the LED on for 1 second (adjust as needed)
      digitalWrite(ledPin3, LOW); // Turn off the LED
    }
    else if (command.indexOf("ground floor") != -1 || command.indexOf("ground") != -1|| command.indexOf("g") != -1) {
      Serial.println("Recognized command: " + command);
      digitalWrite(ledPin0, HIGH); // Turn on the LED
      delay(10000); // Keep the LED on for 1 second (adjust as needed)
      digitalWrite(ledPin0, LOW); // Turn off the LED
    }
  }
}*/
// Define the pin for the button
const int buttonPin = 26;
bool buttonPressed = false;

void setup() {
  // Initialize the button pin as an input
  pinMode(buttonPin, INPUT_PULLUP);
  // Initialize the LED pins as outputs

  // Start serial communication at 9600 baud
  Serial.begin(9600);
}

void loop() {
  // Read the state of the button
  int buttonState = digitalRead(buttonPin);

  // If the button is pressed (LOW), send a signal
  if (buttonState == LOW) {
    if (!buttonPressed) {
      Serial.println("ButtonPressed");
      buttonPressed = true; // Set a flag to avoid multiple presses
      delay(500); // Debounce delay (adjust as needed)
    }
  } else {
    buttonPressed = false; // Reset the flag when the button is released
  }

    // Check for a voice command
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    // Variables to track which floors are mentioned
    int floor;

    // Check if the recognized command contains any of the specified substrings for floors
    if (command.indexOf("first") != -1 || command.indexOf("1") != -1 || command.indexOf("1st") != -1 || command.indexOf("one") != -1) {
      floor = 1;
    }
    if (command.indexOf("2nd") != -1 || command.indexOf("2nd floor") != -1 || command.indexOf("second") != -1 || command.indexOf("2") != -1) {
      floor = 2;
    }
    if (command.indexOf("3rd floor") != -1 || command.indexOf("3 floor") != -1 || command.indexOf("3rd") != -1 || command.indexOf("third") != -1) {
      floor = 3;
    }
    if (command.indexOf("ground floor") != -1 || command.indexOf("ground") != -1 ) {
      floor = 0;
    }

  }
}



