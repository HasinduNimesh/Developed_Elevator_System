/*
   ELEVATOR  BY BN   
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial3
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "Elevator WIFI2"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377

//loadcell
#include<SPI.h>
#include<Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display( OLED_RESET);
#define NUMFLAKES     10 
#include <HX711_ADC.h> // https://github.com/olkal/HX711_ADC

//ultrasonic pins
#define TRIG_PIN 5
#define ECHO_PIN 4

//smoke sensor
#define SMOKE_SENSOR_PIN 23
#define BUZZER_PIN 6

// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 93 bytes
  { 255,2,0,11,0,86,0,16,82,1,3,132,4,60,55,15,2,24,129,0,
  4,8,56,5,65,87,69,76,67,79,77,69,32,84,79,32,69,76,69,86,
  65,84,79,82,0,129,0,8,51,46,10,65,71,32,32,32,49,32,32,32,
  50,32,32,32,51,0,67,5,0,15,64,31,205,24,11,1,2,11,83,41,
  13,1,31,69,109,101,114,103,101,110,99,121,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  uint8_t select_1; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t button_1; // =1 if button pressed, else =0 

    // output variables
  char text_1[11];  // string UTF8 end zero 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////
#include <SoftwareSerial.h>

SoftwareSerial SIM900A(10,11);

#define MOTOR_SPEED 120

const int BUTTON_G = 53;//this is the button to go the ground floor 
const int BUTTON_1 = 51;//this is the button to go the first floor 
const int BUTTON_2 = 49;//this is the button to go the second floor 
const int BUTTON_3 = 47;//this is the button to go the third floor 

const int BUTTON_E = 45;//emergency button
const int LED = 13;//emergency led

//Right motor
int enableRightMotor = 7;
int rightMotorPin1 = 8;
int rightMotorPin2 = 9;

const int IRGPin =  52;//ground floor ir sensor pin
const int IR1Pin =  50;//first floor ir sensor pin
const int IR2Pin =  48;//second floor ir sensor pin
const int IR3Pin =  46;//third floor ir sensor pin 

//variables for ir sensor values
int SensorValueG =0;
int SensorValue1 =0;
int SensorValue2 =0;
int SensorValue3 =0;

char sG[] = "■ G";
char s1[] = "■ 1";
char s2[] = "■ 2";
char s3[] = "■ 3";

char sGu[] = "↑ G";
char s1u[] = "↑ 1";
char s2u[] = "↑ 2";
char s3u[] = "↑ 3";

char sGd[] = "↓ G";
char s1d[] = "↓ 1";
char s2d[] = "↓ 2";
char s3d[] = "↓ 3";

int s_f;//sensor floor
int c_f;//current floor
int d_f;//destination floor
int p_s;//floor that recived from remotexy

bool e =0;

//voice button and variable declaration
const int voicePin = 43;
bool buttonPressed = false;

bool voiceC = false; // Flag to indicate if the elevator is being summoned

//loadcell and oled
HX711_ADC LoadCell(41, 39); // parameters: dt pin, sck pin
int taree = 13;
int a = 0;
float b = 0;


void setup() 
{
  RemoteXY_Init (); 

  TCCR0B = TCCR0B & B11111000 | B00000010;

  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);

  pinMode(IRGPin, INPUT);
  pinMode(IR1Pin, INPUT);
  pinMode(IR2Pin, INPUT);
  pinMode(IR3Pin, INPUT); 

  pinMode(BUTTON_G, INPUT_PULLUP);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  pinMode(BUTTON_3, INPUT_PULLUP);

  pinMode(BUTTON_E, INPUT_PULLUP); 

  pinMode(LED, OUTPUT);
  //voice activating button
  pinMode(voicePin, INPUT_PULLUP);
  //ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  //smoke sensor pin
  pinMode(SMOKE_SENSOR_PIN,INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  //loadcell
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.print("Starting.....");
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  delay(2000);


  pinMode (taree, INPUT);
  LoadCell.begin(); // start connection to HX711
  LoadCell.start(1000); // load cells gets 2000ms of time to stabilize
  LoadCell.setCalFactor(1071.50); // calibration factor for load cell => strongly dependent on your individual setup

  display.clearDisplay();

  rotateMotor(0);

  Serial.begin(9600);
  SIM900A.begin(9600); 
  
  
}


int destinationFloorGetter(){

  int buttonStateG = digitalRead(BUTTON_G);
  int buttonState1 = digitalRead(BUTTON_1);
  int buttonState2 = digitalRead(BUTTON_2);
  int buttonState3 = digitalRead(BUTTON_3);

  if(buttonStateG == LOW && e==0){
    e=1;         
    d_f =0;
  }
  else if(buttonState1 == LOW && e==0){ 
    e=1;        
    d_f =1;
  }
  else if(buttonState2 == LOW && e==0){ 
     e=1;        
    d_f =2;
  }
  else if(buttonState3 == LOW && e==0){ 
     e=1;        
    d_f =3;
  }
  else if(e==0){
    e=1;
    d_f=p_s;
  }
  return d_f;
}

void elevatorDirector(int d_f){
  
  SensorValueG = digitalRead(IRGPin);
  SensorValue1 = digitalRead(IR1Pin);
  SensorValue2 = digitalRead(IR2Pin);
  SensorValue3 = digitalRead(IR3Pin);  

  if (SensorValueG==LOW){ // LOW MEANS Object Detected
    s_f=0;       
  }
  else if (SensorValue1==LOW){ // LOW MEANS Object Detected
    s_f=1;       
  }
  else if (SensorValue2==LOW){ // LOW MEANS Object Detected
    s_f=2;           
  }
  else if (SensorValue3==LOW){ // LOW MEANS Object Detected
    s_f=3;            
  }

  switch (d_f) {
  case 0:
    down0();
    break;
  case 1:
    if(s_f<d_f){
      up1();
    }
    else{
      down1();
    }
    break;
  case 2:
     if(s_f<d_f){
      up2();
    }
    else{
      down2();
    }
    break;
  case 3:
    up3();
    break;
  default:
    // statements
    break;
 }
}

int voice(){
  // Read the state of the button
  int buttonState = digitalRead(voicePin);

  // If the button is pressed (LOW), send a signal
  if (buttonState == LOW && e==0)  {
    if (!buttonPressed) {
      Serial.println("ButtonPressed");
      e=1;
      buttonPressed = true; // Set a flag to avoid multiple presses
       delay(500);           // Debounce delay (adjust as needed)
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
    if (command.indexOf("ground floor") != -1 || command.indexOf("ground") != -1) {
      floor = 0;
    }

   
    Serial.println(floor);
    
    return floor;
    //voiceC=true;
  }


}

void SendMessage(String s)
{
 
  SIM900A.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  
  RemoteXY_delay(1000); 
 
  SIM900A.println("AT+CMGS=\"+94750783355\"\r"); //Mobile phone number to send message
  
  RemoteXY_delay(1000);
 
  SIM900A.println(s);// Messsage content
  
  RemoteXY_delay(100);
  
  SIM900A.println((char)26);// ASCII code of CTRL+Z
  
  RemoteXY_delay(1000);
 
}


void rotateMotor(int rightMotorSpeed) {

  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, HIGH);
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH);
    digitalWrite(rightMotorPin2, LOW);
  } else {
    digitalWrite(rightMotorPin1, LOW);
    digitalWrite(rightMotorPin2, LOW);
  }
  analogWrite(enableRightMotor, abs(rightMotorSpeed));  
}

void up3() {
  if (SensorValueG==LOW){ // LOW MEANS Object Detected
    c_f=0;
    strcpy  (RemoteXY.text_1, sGu);       
  }
  else if (SensorValue1==LOW){ // LOW MEANS Object Detected
    c_f=1;
    strcpy  (RemoteXY.text_1, s1u);       
  }
  else if (SensorValue2==LOW){ // LOW MEANS Object Detected
    c_f=2;
    strcpy  (RemoteXY.text_1, s2u);            
  }
  else if (SensorValue3==LOW){ // LOW MEANS Object Detected
    c_f=3;
    strcpy  (RemoteXY.text_1, s3);    
    rotateMotor(0); 
    RemoteXY.select_1=3;
    e=0;      
  }
  if (SensorValue3==HIGH){ // LOW MEANS Object Detected
    
    rotateMotor(MOTOR_SPEED);
  }
  else
  {            
  }
}

void down0()
{
   if (SensorValueG==LOW){ // LOW MEANS Object Detected
    c_f=0;
    strcpy  (RemoteXY.text_1, sG);    
     rotateMotor(0);
     RemoteXY.select_1=0;
    e=0;         
  }
  else if (SensorValue1==LOW){ // LOW MEANS Object Detected
    c_f=1;
    strcpy  (RemoteXY.text_1, s1d);       
  }
  else if (SensorValue2==LOW){ // LOW MEANS Object Detected
    c_f=2;
    strcpy  (RemoteXY.text_1, s2d);           
  }
  else if (SensorValue3==LOW){ // LOW MEANS Object Detected
    c_f=3; 
    strcpy  (RemoteXY.text_1, s3d);          
  }
  if (SensorValueG==HIGH){ // LOW MEANS Object Detected
    
    rotateMotor(-MOTOR_SPEED);
  }
  else
  {          
  }
}

void up1(){

   if (SensorValueG==LOW){ // LOW MEANS Object Detected
    c_f=0;
    strcpy  (RemoteXY.text_1, sGu);       
  }
  else if (SensorValue1==LOW){ // LOW MEANS Object Detected
    c_f=1;
    strcpy  (RemoteXY.text_1, s1);    
    rotateMotor(0); 
    RemoteXY.select_1=1;
    e=0;        
  }
  else if (SensorValue2==LOW){ // LOW MEANS Object Detected
    c_f=2;
    strcpy  (RemoteXY.text_1, s2u);            
  }
  else if (SensorValue3==LOW){ // LOW MEANS Object Detected
    c_f=3;
    strcpy  (RemoteXY.text_1, s3u);               
  }
  if (SensorValue1==HIGH){ // LOW MEANS Object Detected
    
    rotateMotor(MOTOR_SPEED);
  }
  else
  {            
  }

}
void down1(){

  if (SensorValueG==LOW){ // LOW MEANS Object Detected
    c_f=0;
    strcpy  (RemoteXY.text_1, sGd);           
  }
  else if (SensorValue1==LOW){ // LOW MEANS Object Detected
    c_f=1;
    strcpy  (RemoteXY.text_1, s1);    
    rotateMotor(0);
    RemoteXY.select_1=1;
    e=0;           
  }
  else if (SensorValue2==LOW){ // LOW MEANS Object Detected
    c_f=2;
    strcpy  (RemoteXY.text_1, s2d);           
  }
  else if (SensorValue3==LOW){ // LOW MEANS Object Detected
    c_f=3;
    strcpy  (RemoteXY.text_1, s3d);          
  }
  if (SensorValue1==HIGH){ // LOW MEANS Object Detected
    
    rotateMotor(-MOTOR_SPEED);
  }
  else
  {        
  }

}

void up2(){

   if (SensorValueG==LOW){ // LOW MEANS Object Detected
    c_f=0; 
    strcpy  (RemoteXY.text_1, sGu);      
  }
  else if (SensorValue1==LOW){ // LOW MEANS Object Detected
    c_f=1;
    strcpy  (RemoteXY.text_1, s1u);       
  }
  else if (SensorValue2==LOW){ // LOW MEANS Object Detected
    c_f=2;
    strcpy  (RemoteXY.text_1, s2);    
    rotateMotor(0); 
    RemoteXY.select_1=2;
    e=0;               
  }
  else if (SensorValue3==LOW){ // LOW MEANS Object Detected
    c_f=3;
    strcpy  (RemoteXY.text_1, s3u);               
  }
  if (SensorValue2==HIGH){ // LOW MEANS Object Detected
    
    rotateMotor(MOTOR_SPEED);
  }
  else
  {            
  }

}
void down2(){

  if (SensorValueG==LOW){ // LOW MEANS Object Detected
    c_f=0;
    strcpy  (RemoteXY.text_1, sGd);           
  }
  else if (SensorValue1==LOW){ // LOW MEANS Object Detected
    c_f=1;
    strcpy  (RemoteXY.text_1, s1d);       
  }
  else if (SensorValue2==LOW){ // LOW MEANS Object Detected
    c_f=2; 
    strcpy  (RemoteXY.text_1, s2);   
    rotateMotor(0); 
    RemoteXY.select_1=2;
    e=0;              
  }
  else if (SensorValue3==LOW){ // LOW MEANS Object Detected
    c_f=3;
    strcpy  (RemoteXY.text_1, s3d);      
   
  }
  if (SensorValue2==HIGH){ // LOW MEANS Object Detected
    
    rotateMotor(-MOTOR_SPEED);
  }
  else
  {           
  }
}

int ultrasonic() {
  long duration, distance;
  
  // Clear the trigger pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  
  // Set the trigger pin high for 10 microseconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Measure the pulse input in the echo pin
  duration = pulseIn(ECHO_PIN, HIGH);
  
  // Calculate the distance
  distance = duration * 0.034 / 2;
  
  // If an object is detected within 100cm, return 1
  if (distance < 10) {
    return 1;
  } else {
    return 0;
  }
}

int smoke() {
  int sensorValue = digitalRead(SMOKE_SENSOR_PIN);
  
  // If smoke is detected, return 1
  if (sensorValue == HIGH) {
    return 1;
    SendMessage("Fire or Smoke Detected !!!");
  } else {
    return 0;
  }
}
//display weight function
void displayWeight() {
  // Display header
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 1);
  display.println("Weight machine");

  // Retrieve data from the load cell
  LoadCell.update();
  int weight = (int)LoadCell.getData();

  // Check for negative weight
  if (weight < 0) {
    weight = -weight;
    display.setCursor(10, 1);
    display.print("-");
  }

  // Display weight value
  display.setCursor(10, 12);
  display.setTextSize(2);
  display.print("Value:");
  display.setCursor(60, 12);
  display.print(weight, 1);
  display.print("g");

  // Check for overload
  if (weight >= 200) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 1);
    display.println("OVER LOAD");
  }

  // Update the OLED display
  display.display();
  display.clearDisplay();
}

//time getter
String formatTime(unsigned long millisecs) {
  unsigned long seconds = millisecs / 1000;
  unsigned long minutes = seconds / 60;
  unsigned long hours = minutes / 60;

  seconds %= 60;
  minutes %= 60;

  char buffer[20];
  sprintf(buffer, "%02lu:%02lu:%02lu", hours, minutes, seconds);

  return String(buffer);
}


void loop() {
  RemoteXY_Handler();

  p_s = RemoteXY.select_1;


  // Check if the voice recognition button is pressed
  int buttonState = digitalRead(voicePin);
  if(buttonState==LOW){
    voiceC=true;
  }

  
  if(voiceC == false){
    if(ultrasonic()==1){
          elevatorDirector(destinationFloorGetter());
    }
  }else{
    elevatorDirector(voice());
    //voiceC=true;
  }

  
  int buttonStateE = digitalRead(BUTTON_E);
  if(buttonStateE==LOW){         
     digitalWrite (LED, HIGH);
     SendMessage("Emergency! Time: " + formatTime(millis()));
  }
  else if( RemoteXY.button_1==1){         
    digitalWrite (LED, HIGH);
    SendMessage("Emergency! Time: " + formatTime(millis()));
  }
  else if(buttonStateE==HIGH ){         
     digitalWrite (LED, LOW);
  }
  else if( RemoteXY.button_1==0){         
     digitalWrite (LED, LOW);
  }
//smoke detect
   int smokeDetected = smoke();
  
  if (smokeDetected) {
    digitalWrite(BUZZER_PIN, LOW); // Turn on the buzzer
  } else {
    digitalWrite(BUZZER_PIN, HIGH); // Turn off the buzzer
  }
 // displayWeight();

}




