// Constants
const int triggerPin = 2;       // The trigger pin of the ultrasonic sensor
const int echoPin = 3;          // The echo pin of the ultrasonic sensor
const int buttonPin = 4;        // The button pin to simulate the elevator button
const int doorControlPin = 5;   // The pin controlling the elevator door (simulated using an LED)

// Variables
long duration;
int distance;

void setup() {
  // Initialize pins
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(doorControlPin, OUTPUT);

  // Start serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Send a pulse to the ultrasonic sensor to trigger a measurement
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Read the duration of the echo signal
  duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in centimeters
  distance = duration * 0.034 / 2;

  // Read the elevator button state
  boolean buttonState = digitalRead(buttonPin);

  // Check if there's an object in front of the elevator within a certain range
  if (distance < 100) {  // Adjust the threshold distance as needed
    Serial.println("Object detected in front of the elevator at a distance of " + String(distance) + " cm.");
   buttonState = digitalRead(buttonPin);
    // Check if the elevator button is pressed
    if (buttonState == LOW) {
      Serial.println("Elevator button pressed. Elevator is responding.");

      // Simulate opening the elevator doors (simulated using an LED)
      digitalWrite(doorControlPin, HIGH);
      delay(10000);
    } if (buttonState == HIGH){
      // Close the elevator doors
      Serial.println("Elevator button relessed.");
      digitalWrite(doorControlPin, LOW);
    }
  } else {
    Serial.println("No object detected.");
    // Close the elevator doors
    digitalWrite(doorControlPin, LOW);
  }

  // Add a delay to prevent rapid button presses
  delay(1000);
}