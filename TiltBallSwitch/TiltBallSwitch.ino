/*
    Tilt Ball Switch

    Indicate through the builtin LED the position of the tilt ball switch. 
    Upper 30 angle to both sides = LED On. Other angles = LED Off. 
*/

// remove this line if using Arduino IDE
#include <Arduino.h>

int ledPin = 13;
int tiltSwitchPin = 2;

// tilt ball switch value
int val;

void  setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(tiltSwitchPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void  loop()
{ 
    val = digitalRead(tiltSwitchPin);

    // Now invert the output on the ledPin
    if (val==HIGH)
    {
      digitalWrite(ledPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, HIGH);
    }
    
    Serial.println(val);
    delay(500);
}
