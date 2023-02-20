/*
    Stepper Motor 28BYJ-48

    Shaft of the stepper motor rotates one full revolution clockwise, then CCW.

    Connections (ULN2003 - ARDUINO):
    * IN1 --> 11
    * IN2 --> 10
    * IN3 --> 9
    * IN4 --> 8
    * VCC --> 5V
    * GND --> GND
 */

// remove this line if using Arduino IDE
#include <Arduino.h>

#include <Stepper.h>

const int stepsPerRevolution = 2048; // 32 teeth on cogged wheel * 64 gear ratio = 2048

// initialize the stepper library on pins 8 through 11 (mind the order!)
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() 
{
  Serial.begin(9600);
  // set the speed; minimum delay between steps is around 2ms
  myStepper.setSpeed(10);// goes up to 18 for 28BYJ-48 + ULN2003
}

void loop() 
{
   // step one revolution  in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);

  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
}