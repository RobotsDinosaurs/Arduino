#include <Servo.h>

int servoPin = 9;
int angle = 0;

Servo servo;

void setup() {
  servo.attach(servoPin);
}

void loop() {
  for (angle = 0; angle <= 180; angle += 5) { 
    servo.write(angle);
    delay(150);
  }
  
  for (angle = 180; angle >= 0; angle -= 5) { 
    servo.write(angle);
    delay(150);
  }
}
