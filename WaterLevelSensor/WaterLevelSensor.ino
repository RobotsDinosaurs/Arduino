// Sensor pins
#define sensorPower 7
#define sensorPin A0

// Value for storing water level
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


//However, one commonly known issue with these sensors is their short lifespan when exposed to a moist environment. 
//Having power applied to the probe constantly speeds the rate of corrosion significantly.
//To overcome this, we recommend that you do not power the sensor constantly, but power it only when you take the readings.

void loop()
{
    digitalWrite(sensorPower, HIGH);  // Turn the sensor ON
    delay(10);              // wait 10 milliseconds
  
    value = analogRead(sensorPin); // get adc value
    sprintf(printBuffer, "Value: %d", value);      
    Serial.println(printBuffer);
    
    digitalWrite(sensorPower, LOW);   // Turn the sensor OFF

    if (((HistoryValue>=value) && ((HistoryValue - value) > margin_allowed)) || ((HistoryValue<value) && ((value - HistoryValue) > margin_allowed)))
    {
      if (value<=120){ 
        Serial.println("Water level: Empty!"); 
      }
      else if (value>120 && value<=135){ 
        Serial.println("Water level: 0 to 0.5 inch"); 
      }
      else if (value>135 && value<=150){ 
        Serial.println("Water level: 0.5 to 1 inch"); 
      }
      else if (value>150 && value<=165){ 
        Serial.println("Water level: 1 to 1.5 inch"); 
      }
      else if (value>165){ 
        Serial.println("Water level: 1.5 to 1.7 inches"); 
      }

      HistoryValue = value;
    }
    
    delay(5000);
}
