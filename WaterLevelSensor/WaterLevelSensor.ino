/*
  WaterLevelSensor

  Gets a reading from a sensor and outputs the value and percentage of the level of water 
  on a serial monitor. 
*/

#include <Arduino.h>

// Sensor pins
#define sensorPower 5
#define sensorPin A0

// Value for storing water level
int value = 0;
char printBuffer[128];

void setup() {
  pinMode(sensorPower, OUTPUT);
  
  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);
  
  Serial.begin(9600);
}

void loop()
{
    digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
    delay(1000);              // wait 1000 milliseconds
  
    value = analogRead(sensorPin); // get adc value
    sprintf(printBuffer, "Value: %d", value);      
    
    //values based on calibration
    if (value<=120){ 
      sprintf(printBuffer+strlen(printBuffer), "= 0%%");
    }
    else if (value>120 && value<=135){ 
      sprintf(printBuffer+strlen(printBuffer), "= 0%% - 25%%");
    }
    else if (value>135 && value<=145){ 
      sprintf(printBuffer+strlen(printBuffer), "= 25%% - 50%%");
    }
    else if (value>145 && value<=150){ 
      sprintf(printBuffer+strlen(printBuffer), "= 50%% - 75%%");
    }
    else if (value>150){ 
      sprintf(printBuffer+strlen(printBuffer), "= 75%% - 100%%");
    }

    Serial.println(printBuffer);
    
    // Do not power the sensor constantly, but power it only when you take the readings.  
    digitalWrite(sensorPower, LOW);   // Turn the sensor OFF
    
    delay(5000);
}