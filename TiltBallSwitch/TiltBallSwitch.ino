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
