#define BUZZER_PIN 2
#define SMOKE_THRESHOLD 80  // Adjust this threshold based on your sensor and environment

void setup() 
{
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() 
{
  int sensorValue = analogRead(A5);
  Serial.println(sensorValue);

  if (sensorValue > SMOKE_THRESHOLD)
  {
    digitalWrite(BUZZER_PIN, HIGH); // Turn on the buzzer
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW); // Turn off the buzzer   
  }
  
  delay(1000); // Adjust the delay based on your requirements
}
