/*
    IR Remote

    Print out the received IR value. Turn on the red LED if received a specific code.
*/

// remove this line if using Arduino IDE
#include <Arduino.h>

#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 7;
const int redPin = 10;

void setup()
{
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
  pinMode(redPin, OUTPUT);
}

void loop(){
  if (IrReceiver.decode()) 
  {
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
    IrReceiver.printIRResultShort(&Serial);

    switch(IrReceiver.decodedIRData.decodedRawData)
    {
      case 0xE31CFF00: //Keypad button "5"
        Serial.println("5");
        digitalWrite(redPin, HIGH);
        delay(2000);
        digitalWrite(redPin, LOW);
        break;
      default:
        break;
    }

    IrReceiver.resume(); // Enable receiving of the next value
  }
}