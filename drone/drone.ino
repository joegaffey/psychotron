#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

const int FWD_LEFT = 2;
const int FWD_RIGHT = 5;
const int BACK_LEFT = 3;
const int BACK_RIGHT = 4;
const int LED = 13;
const int TRIG = 10;
const int ECHO = 9;

int distance = -1;

void forward() {
    digitalWrite(FWD_LEFT, HIGH);
    digitalWrite(FWD_RIGHT, HIGH);
}

void right() {
    digitalWrite(BACK_LEFT, HIGH);
    digitalWrite(FWD_RIGHT, HIGH);
}

void backwards() {
    digitalWrite(BACK_LEFT, HIGH);
    digitalWrite(BACK_RIGHT, HIGH);
}

void left() {
    digitalWrite(FWD_LEFT, HIGH);
    digitalWrite(BACK_RIGHT, HIGH);
}

void stopRobot() {
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
    return pulseIn(ECHO, HIGH, 30000)/58.0;
}

void ledOn() {
    digitalWrite(LED, HIGH);
}

void ledOff() {
    digitalWrite(LED, LOW);
}

String raw = "";
int action = -1;
int exTime = 50;

void readAction() {
    if (Serial.available() > 0) {
        raw = Serial.readStringUntil(';');
        if(raw.length() > 2) {
            action = raw.substring(2,3).toInt();
        }
        if(raw.length() > 3) {
            exTime = raw.substring(3, raw.length()).toInt();
        }
    }
    else
      action = -1;
}

void setup() {
    pinMode(FWD_LEFT, OUTPUT);
    pinMode(FWD_RIGHT, OUTPUT);
    pinMode(BACK_LEFT, OUTPUT);
    pinMode(BACK_RIGHT, OUTPUT);
    pinMode(LED, OUTPUT);
    Serial.begin(115200);
}

void ping() {
  Serial.println((int)getDistance());
}

void loop() {
    readAction();
    if(action > -1) {
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
    int currDistance = (int)getDistance();
    if(currDistance != distance) {
      distance = currDistance;
      Serial.println(distance);
    }
    delay(exTime);
    exTime = 50;
    action = -1;    
}
