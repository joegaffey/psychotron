#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

/* Lego robot pins */
const int FWD_LEFT = 2;
const int FWD_RIGHT = 5;
const int BACK_LEFT = 3;
const int BACK_RIGHT = 4;
const int LED = 13;
const int TRIG = 8;
const int ECHO = 7;
const int ENA = 9;
const int ENB = 10;
const int LINE_SENSOR_1 = 11;
const int LINE_SENSOR_2 = 12;

/* Robot actions in */
const String ACTION_STOP = "STOP";
const String ACTION_FWD = "FWD";
const String ACTION_BACK = "BACK";
const String ACTION_LEFT = "LEFT";
const String ACTION_RIGHT = "RIGHT";
const String ACTION_LEDON = "LEDON";
const String ACTION_LEDOFF = "LEDOFF";
const String ACTION_PING = "PING";

/* Serial messages out*/
const int COLLISION_MESSAGE = -1;
const int LINE_DETECTED_MESSAGE = -2;

/* Misc settings */
const int MINIMUM_DISTANCE = 5;
const int DEBUG_MODE = true;
const int SERIAL_PORT_SPEED = 9600;
const int SPEED = 128;

/* Globals */
int distance = 999;
bool running = true;

void forward() {
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(FWD_LEFT, HIGH);
  digitalWrite(FWD_RIGHT, HIGH);
}

void right() {
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(BACK_LEFT, HIGH);
  digitalWrite(FWD_RIGHT, HIGH);
}

void backwards() {
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(BACK_LEFT, HIGH);
  digitalWrite(BACK_RIGHT, HIGH);
}

void left() {
  analogWrite(ENA, SPEED);
  analogWrite(ENB, SPEED);
  digitalWrite(FWD_LEFT, HIGH);
  digitalWrite(BACK_RIGHT, HIGH);
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
  if(digitalRead(LINE_SENSOR_1) == HIGH && digitalRead(LINE_SENSOR_2) == HIGH)
      return true;
  return false;
}

void ledOn() {
  digitalWrite(LED, HIGH);
}

void ledOff() {
  digitalWrite(LED, LOW);
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
}

void loop() {
  if(running)
    handleAction(readActionFromSerial());

  int tmpDistance = (int)getDistance();
  if(tmpDistance != distance && tmpDistance > 0) { //Only address changes in distance
    distance = tmpDistance;
    Serial.println(distance);
    if(distance < MINIMUM_DISTANCE)
      handleCollision();
  }

  if(lineDetected()) {
    handleLinedDetected();
  }

  delay(50);
}
