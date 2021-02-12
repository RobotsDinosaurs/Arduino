/*
  CapacitiveMoistureSensor with LCD Display

  Gets a reading from a sensor and outputs percentage (soil moisture) 
  and condition ("Wet"/"Moist"/"Dry") on an LCD display and serial monitor. 
*/

// remove this line if using Arduino IDE
#include <Arduino.h>

#include <Wire.h>
#include <LiquidCrystal.h>

// Sensor pins
#define sensorPower 5
#define sensorPin A0

int value = 0;
char printBuffer[128];

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// moisture sensor calibration values
const int airValue = 590;   // value in the air 
const int waterValue = 290;  // value in a cup full of water
int soilMoisturePercentage = 0;

void setup()
{
  pinMode(sensorPower, OUTPUT);
  
  // Set to LOW so no power flows through the sensor
  digitalWrite(sensorPower, LOW);
  
  Serial.begin(9600);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Soil Moisture:");
}

void loop()
{
    digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
    delay(1000);              // wait 1000 milliseconds
  
    value = analogRead(sensorPin); // get adc value
    sprintf(printBuffer, "Value: %d", value);      
    Serial.println(printBuffer);

    // Do not power the sensor constantly, but power it only when you take the readings.    
    digitalWrite(sensorPower, LOW);   // Turn the sensor OFF

    soilMoisturePercentage = map(value, airValue, waterValue, 0, 100);
    sprintf(printBuffer, "%d%%", soilMoisturePercentage);      
      
    int interval = (airValue - waterValue)/3; // interval for 3 soil moisture values : Dry/Moist/Wet

    if (value<=waterValue + interval){ 
      sprintf(printBuffer + strlen(printBuffer), " Wet!  "); 
    }
    else if (value>waterValue+interval && value<=airValue - interval){ 
      sprintf(printBuffer + strlen(printBuffer), " Moist  "); 
    }
    else if (value>airValue - interval){ 
      sprintf(printBuffer + strlen(printBuffer), " Dry!  "); 
    }

    Serial.println(printBuffer);
    // set the cursor to column 0, line 1
    // (note: line 1 is the second row, since counting begins with 0):
    lcd.setCursor(0, 1);
    // print the soil moisture percentage and condition
    lcd.print(printBuffer);
    
    delay(5000);
}