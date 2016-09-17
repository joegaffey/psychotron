#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

/* Lego robot pins */
const int FRONT_LEFT = 2;
const int FRONT_RIGHT = 5;
const int BACK_LEFT = 3;
const int BACK_RIGHT = 4;
const int LED = 13;
const int TRIG = 10;
const int ECHO = 9;
const int ENA = 11;
const int ENB = 12;
const int LINE_SENSORS[] = {6, 7};

/* Elegoo robot pins
const int FRONT_LEFT = 2;
const int FRONT_RIGHT = 5;
const int BACK_LEFT = 3;
const int BACK_RIGHT = 4;
const int LED = 13;
const int TRIG = 10;
const int ECHO = 9;
const int[] LINE_SENSORS = {6, 7, 8};
*/

/* Misc settings */
const int MINIMUM_DISTANCE = 8;
const int DEBUG_MODE = false;
const int SERIAL_PORT_SPEED = 9600;

/* Serial messages */
const int COLLISION_MESSAGE = -1;
const int LINE_DETECTED_MESSAGE = -2;

/* Globals */
int distance = -1;
bool running = true;

void forward() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(FRONT_LEFT, HIGH);
  digitalWrite(FRONT_RIGHT, HIGH);
}

void right() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(BACK_LEFT, HIGH);
  digitalWrite(FRONT_RIGHT, HIGH);
}

void backwards() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(BACK_LEFT, HIGH);
  digitalWrite(BACK_RIGHT, HIGH);
}

void left() {
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(FRONT_LEFT, HIGH);
  digitalWrite(BACK_RIGHT, HIGH);
}

void stopRobot() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  digitalWrite(FRONT_LEFT, LOW);
  digitalWrite(BACK_LEFT, LOW);
  digitalWrite(BACK_RIGHT, LOW);
  digitalWrite(FRONT_RIGHT, LOW);
}

float getDistance() {
  pinMode(TRIG, OUTPUT);
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  pinMode(ECHO, INPUT);
  return pulseIn(ECHO, HIGH, 30000) / 58.0;
}

boolean lineDetected() {
  for(int i = 0; i < sizeof(LINE_SENSORS); i++) {
    if(digitalRead(LINE_SENSORS[i]) == HIGH)
      return true;
  }
  return false;
}

void ledOn() {
  digitalWrite(LED, HIGH);
}

void ledOff() {
  digitalWrite(LED, LOW);
}

int readActionFromSerial() {
  if (Serial.available() > 0) 
    return Serial.readStringUntil('\n').toInt();
  else
    return -1;
}

void ping() {
  Serial.println((int)getDistance());
}

void handleAction(int action) {
  switch(action) {
    case 0:
        stopRobot();
        break;
    case 1:
        forward();
        break;
    case 2:
        backwards();
        break;
    case 3:
        left();
        break;
    case 4:
        right();
        break;
    case 5:
        ledOn();
        break;
    case 6:
        ledOff();
        break;
    case 7:
        ping();
        break;
    default:
        break;
  }
}

void handleCollision() {
  stopRobot();
  Serial.println(COLLISION_MESSAGE);
  if(!DEBUG_MODE)
    running = false;
}

void handleLinedDetected() {
  stopRobot();
  Serial.println(LINE_DETECTED_MESSAGE);
  if(!DEBUG_MODE) 
    running = false;
}

void setupLineSensors() {
  for(int i = 0; i < sizeof(LINE_SENSORS); i++) {
    pinMode(LINE_SENSORS[i], INPUT);
  }
}

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(FRONT_LEFT, OUTPUT);
  pinMode(FRONT_RIGHT, OUTPUT);
  pinMode(BACK_LEFT, OUTPUT);
  pinMode(BACK_RIGHT, OUTPUT);
  pinMode(LED, OUTPUT);
  setupLineSensors();       
  Serial.begin(SERIAL_PORT_SPEED);
}

void loop() {
  int action = readActionFromSerial();
  if(running && action > -1) 
    handleAction(action);
  
  int tmpDistance = (int)getDistance();
  if(tmpDistance == 0 || tmpDistance == 3) //Filter out false 0's and 3's - NB: replace that sensor!!! 
    ;
  else if(tmpDistance != distance && tmpDistance > 0) { //Only address changes in distance 
    distance = tmpDistance;
    Serial.println(distance);
    if(distance < MINIMUM_DISTANCE)
      handleCollision();
  }   
  
  if(lineDetected) {
    handleLinedDetected();
  }
  
  delay(100);
}
