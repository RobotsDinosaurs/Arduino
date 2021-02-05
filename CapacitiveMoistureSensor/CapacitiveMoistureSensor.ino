/*
  CapacitiveMoistureSensor

  Gets a reading from a sensor and outputs "Wet"/"Moist"/"Dry" on a serial monitor. 

  YT link: TODO
*/

#include <Arduino.h>

// Sensor pins
#define sensorPower 7
#define sensorPin A0

int value = 0;
int HistoryValue = 0;
char printBuffer[128];
int margin_allowed = 2;

void setup()
{
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
    sprintf(printBuffer, "Value: %d", value);      
    Serial.println(printBuffer);

    // Do not power the sensor constantly, but power it only when you take the readings.    
    digitalWrite(sensorPower, LOW);   // Turn the sensor OFF

    if(((HistoryValue>=value) && ((HistoryValue - value) > margin_allowed)) || ((HistoryValue<value) && ((value - HistoryValue) > margin_allowed)))
    {
      // moisture sensor - 200 for nothing, 155 for cup full
      if (value<=170){ 
        Serial.println("Wet!"); 
      }
      else if (value>170 && value<=185){ 
        Serial.println("Moist"); 
      }
      else if (value>185){ 
        Serial.println("Dry!"); 
      }
      
      HistoryValue = value;
    }
    
    delay(5000);
}