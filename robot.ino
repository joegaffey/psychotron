#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>
#include <SoftwareSerial.h>


void Forward();
void Right();
void Backwards();
void Stop();
void Left();

void Forward() {
    Serial.println("Forward");
    digitalWrite(2,1);
    digitalWrite(5,1);
}

void Right() {
    Serial.println("Right");
    digitalWrite(3,1);
    digitalWrite(5,1);
}

void Backwards() {
    Serial.println("Backwards");
    digitalWrite(3,1);
    digitalWrite(4,1);
}

void Left() {
    Serial.println("Left");
    digitalWrite(2,1);
    digitalWrite(4,1);
}

void Stop() {
    Serial.println("Stop");
    digitalWrite(2,0);
    digitalWrite(3,0);
    digitalWrite(4,0);
    digitalWrite(5,0);
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

int action = 0;

void roam() {
    Serial.println("Roam");

    while(true) {
        ledOff();
        readAction();
        if(action == 8) 
          break;
        
        clearBuffer();

        readAction();
        if(action == 8) 
          break;
        
        delay(1000*0.1);

        readAction();
        if(action == 8) 
          break;
        
        if(getDistance(10,9) > 25){
            Forward();
        }
        else {
            ledOn();
            Stop();
            if(random(0,1) == 1){
                Right();
                delay(1000*(3) / (random(1,6)));
            }
            else {
                Left();
                delay(1000*(3) / (random(1,6)));
            }
            Stop();
        }
    }    
    Stop();
}

void clearBuffer() {
    while (Serial.available() > 0) {
        Serial.read();
    }
}

void ledOn() {
    digitalWrite(13,1);
}

void ledOff() {
    digitalWrite(13,0);
}

String raw = "";

void readAction() {
    Serial.print("ReadAction: ");

    if (Serial.available() > 0) {

        raw = Serial.readStringUntil('#');
        Serial.println(raw);

        if(raw.length() > 2) {
            action = raw.substring(2,3).toInt();
            Serial.println(action);
        }
    }
}

void setup() {
    pinMode(2,OUTPUT);
    pinMode(5,OUTPUT);
    pinMode(3,OUTPUT);
    pinMode(4,OUTPUT);
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
            case 7:
                roam();
                break;     
            default:
                Stop();  
                break;
        }
    }
}
