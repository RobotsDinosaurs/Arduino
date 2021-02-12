/*
    Servo with Pot

    Mapping the servo sweep to the potentiometer's movement. 
*/

// remove this line if using Arduino IDE
#include <Arduino.h>

#include <Servo.h>

int potPin = 0;
int servoPin = 9;
int angle = 0;

Servo servo;

void setup() {
  servo.attach(servoPin);
  Serial.begin(9600);
}

void loop() {
  int reading = analogRead(potPin);     // 0 to 1023 (Arduino has a 10-bit analog to digital converter)
  
  angle = map(reading, 0, 1023, 0, 180);  // map 'reading' 
                                          // from current range 0-1023
                                          // to target range 0-180 degrees
  Serial.println(angle);

  servo.write(angle);
  delay(15);
}
