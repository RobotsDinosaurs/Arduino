#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 7;
const int redPin = 10;

void setup(){
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
  pinMode(redPin, OUTPUT);
}

void loop(){
  if (IrReceiver.decode()) {
      Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
      IrReceiver.printIRResultShort(&Serial); // optional use new print version
      switch(IrReceiver.decodedIRData.decodedRawData){
          case 0xE31CFF00: //Keypad button "5"
          Serial.println("5");
          digitalWrite(redPin, HIGH);
          delay(2000);
          digitalWrite(redPin, LOW);
          }
      IrReceiver.resume(); // Enable receiving of the next value
  }
}