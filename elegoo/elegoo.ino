#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

/* Robot pins */
const int FWD_LEFT = 6;
const int FWD_RIGHT = 9;
const int BACK_LEFT = 7;
const int BACK_RIGHT = 8;
const int LED = 13;
const int TRIG = A5;
const int ECHO = A4;
const int ENA = 5;
const int ENB = 10;
const int LINE_SENSOR_1 = 2;
const int LINE_SENSOR_2 = 3;
const int LINE_SENSOR_3 = 4;

/* Robot actions in */
const String ACTION_STOP = "STOP";
const String ACTION_FWD = "FWD";
const String ACTION_BACK = "BACK";
const String ACTION_LEFT = "LEFT";
const String ACTION_RIGHT = "RIGHT";
const String ACTION_LEDON = "LEDON";
const String ACTION_LEDOFF = "LEDOFF";
const String ACTION_PING = "PING";
const String ACTION_RESET = "RESET";
const String ACTION_SPEED = "SPEED";

/* Serial messages out*/
const int COLLISION_MESSAGE = -1;
const int LINE_DETECTED_MESSAGE = -2;

/* Misc settings */
const int MINIMUM_DISTANCE = 5;
const int DEBUG_MODE = false;
const int SERIAL_PORT_SPEED = 9600;

/* Globals */
int distance = 999;
bool running = true;
int speed = 128;

void forward() {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(FWD_LEFT, HIGH);
  digitalWrite(FWD_RIGHT, HIGH);
  digitalWrite(BACK_LEFT, LOW);
  digitalWrite(BACK_RIGHT, LOW);
}

void left() {
//  analogWrite(ENA, speed);
//  analogWrite(ENB, speed);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(BACK_LEFT, HIGH);
  digitalWrite(FWD_RIGHT, HIGH);
  digitalWrite(FWD_LEFT, LOW);
  digitalWrite(BACK_RIGHT, LOW);
}

void backwards() {
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
  digitalWrite(BACK_LEFT, HIGH);
  digitalWrite(BACK_RIGHT, HIGH);
  digitalWrite(FWD_LEFT, LOW);
  digitalWrite(FWD_RIGHT, LOW);  
}

void right() {
//  analogWrite(ENA, speed);
//  analogWrite(ENB, speed);
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  digitalWrite(FWD_LEFT, HIGH);
  digitalWrite(BACK_RIGHT, HIGH);
  digitalWrite(BACK_LEFT, LOW);
  digitalWrite(FWD_RIGHT, LOW);
}

void stopRobot() {
  digitalWrite(ENA, LOW);
  digitalWrite(ENB, LOW);
  digitalWrite(FWD_LEFT, LOW);
  digitalWrite(BACK_LEFT, LOW);
  digitalWrite(BACK_RIGHT, LOW);
  digitalWrite(FWD_RIGHT, LOW);
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
//  Serial.print(digitalRead(LINE_SENSOR_1)); 
//  Serial.print(digitalRead(LINE_SENSOR_2));
//  Serial.println(digitalRead(LINE_SENSOR_3));
  
  if( digitalRead(LINE_SENSOR_1) == HIGH && 
//      digitalRead(LINE_SENSOR_2) == HIGH &&  //Faulty sensor
      digitalRead(LINE_SENSOR_3) == HIGH)
    return true;
  else
    return false;  
}

void ledOn() {
  digitalWrite(LED, HIGH);
}

void ledOff() {
  digitalWrite(LED, LOW);
}

void reset() {
  running = true;
}

String readActionFromSerial() {
  if (Serial.available() > 0) 
    return Serial.readStringUntil('\n');
  else
    return "";
}

void ping() {
  Serial.println((int)getDistance());
}

void handleAction(String action) {
  if(action == ACTION_STOP)
    stopRobot();
  else if(action == ACTION_FWD)
    forward();
  else if(action == ACTION_BACK)
    backwards();
  else if(action == ACTION_LEFT)
    left();
  else if(action == ACTION_RIGHT)
    right();
  else if(action == ACTION_LEDON)
    ledOn();
  else if(action == ACTION_LEDOFF)
    ledOff();
  else if(action == ACTION_PING)
    ping();
  else if(action.substring(0, 5) == ACTION_SPEED) {
    String speedStr = action.substring(6, action.length());
    speed = speedStr.toInt(); 
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

void setup() {
  Serial.begin(SERIAL_PORT_SPEED);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(FWD_LEFT, OUTPUT);
  pinMode(FWD_RIGHT, OUTPUT);
  pinMode(BACK_LEFT, OUTPUT);
  pinMode(BACK_RIGHT, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(LINE_SENSOR_1, INPUT);   
  pinMode(LINE_SENSOR_2, INPUT);
  pinMode(LINE_SENSOR_3, INPUT);      
}

void loop() {  
  String action = readActionFromSerial();
  
  if(!running) {
    if(action == ACTION_RESET)
      reset();  
    else 
      return;    
  }
  handleAction(action);  
  
  int tmpDistance = (int)getDistance();
  //Only send changes in distance 
  if(tmpDistance != distance && tmpDistance > 0) { 
    distance = tmpDistance;
    Serial.println(distance);
    if(distance < MINIMUM_DISTANCE)
      handleCollision();
  }   

  if(lineDetected()) {
    handleLinedDetected();
  }
}
