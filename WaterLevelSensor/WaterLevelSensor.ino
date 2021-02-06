/*
  WaterLevelSensor

  Gets a reading from a sensor and outputs the value and percentage of the level of water on a serial monitor. 

  One commonly known issue with water level sensors is their short lifespan when exposed to a moist environment. 
  Having power applied to the probe constantly speeds the rate of corrosion significantly.
  To overcome this, do not power the sensor constantly, but power it only when you take the readings.

  YT link: TODO
*/

#include <Arduino.h>

// Sensor pins
#define sensorPower 5
#define sensorPin A0

// Value for storing water level
int value = 0;
char printBuffer[128];

// water level sensor calibration values
const int airValue = 0;   // value in the air 
const int waterValue = 250;  // value in a cup full of water
int waterLevelPercentage = 0;

void setup() {
   // Set D7 as an OUTPUT
  pinMode(sensorPower, OUTPUT);
  
  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);
  
  Serial.begin(9600);
}

void loop()
{
    digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
    delay(10);              // wait 10 milliseconds
  
    value = analogRead(sensorPin); // get adc value
    waterLevelPercentage = map(value, airValue, waterValue, 0, 100);
    sprintf(printBuffer, "Value: %d, %d%%", value, waterLevelPercentage);      
    Serial.println(printBuffer);
    
    // Do not power the sensor constantly, but power it only when you take the readings.  
    digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
    
    delay(5000);
}