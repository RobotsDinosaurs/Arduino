/*
    PIR Motion Detector HC-SR501

    Testing a passive infra-red sensor.

    Sensor module is powered up after a minute. During initializing time, it will output 0-3 times. One minute later it comes into standby.

    Keep the surface of the lens from close lighting source and wind, which will introduce interference.

    Sensing range: <120 degree (CCW), within 3 metres (CCW) - 7 metres (CW)
    Delay time: 0.3 min - 5 min (5s CCW - 300 s CW)
    Lock time: 0.2 sec
    Voltage: 5V-20V
    TTL output: 3.3V, 0V
    Trigger methods: L - disable repeat trigger, H - enable repeat trigger

    Connections:
    PIR - Arduino
    VCC - 5V
    OUT - D2
    GND - GND

    LED + 220 ohm resistor connected to D4 and GND
*/

// remove this line if using Arduino IDE
#include <Arduino.h>

#define MINUTE 60000 // for converting milliseconds to a minute
#define SECOND 1000 // for converting milliseconds to a second
 
int ledPin = 4;                // pin for the LED
int pirPin = 2;                // signal pin of the PIR sensor

int lastPirVal = LOW;          // the last value of the PIR sensor
int pirVal;                    // the current value of the PIR sensor

unsigned long myTime;          // number of milliseconds passed since the Arduino started running the code itself
char printBuffer[128];         
 
void setup() {
  pinMode(ledPin, OUTPUT);    // declare LED as output
  pinMode(pirPin, INPUT);     // declare PIR sensor as input
 
  Serial.begin(9600);
}
 
void loop(){
  pirVal = digitalRead(pirPin);  // read current input value

  if (pirVal == HIGH) { // movement detected  
    digitalWrite(ledPin, HIGH);  // turn LED on

    if (lastPirVal == LOW) { // if there was NO movement before
      myTime = millis();
      sprintf(printBuffer, "%lu min %lu sec: Motion detected!", myTime/MINUTE, myTime%MINUTE/SECOND); 
      Serial.println(printBuffer);
      lastPirVal = HIGH;
    }
  } else { // no movement detected
    digitalWrite(ledPin, LOW); // turn LED off

    if (lastPirVal == HIGH){ // if there was a movement before
      myTime = millis();
      sprintf(printBuffer, "%lu min %lu sec: Motion ended!", myTime/MINUTE, myTime%MINUTE/SECOND); 
      Serial.println(printBuffer);
      lastPirVal = LOW;
    }
  }
}