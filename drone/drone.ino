#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>

const int FWD_LEFT = 2;
const int FWD_RIGHT = 5;
const int BACK_LEFT = 3;
const int BACK_RIGHT = 4;
const int LED = 13;


void Forward() {
    Serial.println("Forward");
    digitalWrite(FWD_LEFT,HIGH);
    digitalWrite(FWD_RIGHT,HIGH);
}

void Right() {
    Serial.println("Right");
    digitalWrite(BACK_LEFT,HIGH);
    digitalWrite(FWD_RIGHT,HIGH);
}

void Backwards() {
    Serial.println("Backwards");
    digitalWrite(BACK_LEFT,HIGH);
    digitalWrite(BACK_RIGHT,HIGH);
}

void Left() {
    Serial.println("Left");
    digitalWrite(FWD_LEFT,HIGH);
    digitalWrite(BACK_RIGHT,HIGH);
}

void Stop() {
    Serial.println("Stop");
    digitalWrite(FWD_LEFT,LOW);
    digitalWrite(BACK_LEFT,LOW);
    digitalWrite(BACK_RIGHT,LOW);
    digitalWrite(FWD_RIGHT,LOW);
}

float getDistance(int trig,int echo){
    pinMode(trig,OUTPUT);
    digitalWrite(trig,LOW);
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    pinMode(echo, INPUT);
    return pulseIn(echo,HIGH,30000)/58.0;
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
    Serial.print("ReadAction: ");

    if (Serial.available() > 0) {

        raw = Serial.readStringUntil(';');
        Serial.println(raw);

        if(raw.length() > 2) {
            action = raw.substring(2,3).toInt();
            Serial.println(action);
        }
        if(raw.length() > 3) {
            exTime = raw.substring(3, raw.length()).toInt();
            Serial.println(exTime);
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
                Stop();
                break;
            case 1:
                Forward();
                break;
            case 2:
                Backwards();
                break;
            case 3:
                Left();
                break;
            case 4:
                Right();
                break;
            case 5:
                ledOn();
                break;
            case 6:
                ledOff();
                break;
            case 7:
                Serial.println(getDistance());
            default:
                Stop();
                break;
        }
    }
    if(exTime > 0)
      delayMicroseconds(exTime * 1000);
}
