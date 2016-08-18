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

void forward() {
    digitalWrite(FWD_LEFT,HIGH);
    digitalWrite(FWD_RIGHT,HIGH);
}

void right() {
    digitalWrite(BACK_LEFT,HIGH);
    digitalWrite(FWD_RIGHT,HIGH);
}

void backwards() {
    digitalWrite(BACK_LEFT,HIGH);
    digitalWrite(BACK_RIGHT,HIGH);
}

void left() {
    digitalWrite(FWD_LEFT,HIGH);
    digitalWrite(BACK_RIGHT,HIGH);
}

void stop() {
    digitalWrite(FWD_LEFT,LOW);
    digitalWrite(BACK_LEFT,LOW);
    digitalWrite(BACK_RIGHT,LOW);
    digitalWrite(FWD_RIGHT,LOW);
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

void clearBuffer() {
    while (Serial.available() > 0) {
        Serial.read();
    }
}

void ledOn() {
    digitalWrite(LED,1);
}

void ledOff() {
    digitalWrite(LED,0);
}

String raw = "";
int action = 0;
int exTime = 0;

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
}

void setup() {
    pinMode(FWD_LEFT, OUTPUT);
    pinMode(FWD_RIGHT, OUTPUT);
    pinMode(BACK_LEFT, OUTPUT);
    pinMode(BACK_RIGHT, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    readAction();
    if(action > -1) {
        switch(action) {
            case 0:
                stop();
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
                Serial.println(getDistance());
                break;
            default:
                stop();
                break;
        }
    }
    if(exTime > 0)
      delayMicroseconds(exTime * 1000);
    action = 0;
}
