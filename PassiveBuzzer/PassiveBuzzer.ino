int buzzerPin = 12;
 
int numTones = 8;
int tones[] = {523,    587, 659, 698, 784, 880, 988, 1047 };
//            Alto Do  Re    Mi   Fa   So  La   Si  Treble Do
 
void setup()
{
}
 
void loop()
{
  for (int i = 0; i < numTones; i++) 
  {
    tone(buzzerPin, tones[i], 400); // generates a square wave of the specified frequency on buzzerPin for 400 ms
    delay(500); // wait for 100 ms between subsequent sounds
  }
  delay(1000); // wait for 1 second before playing sounds backwards

  for (int j = numTones - 1; j >= 0; j--) 
  {
     tone(buzzerPin, tones[j], 400); // generates a square wave of the specified frequency on buzzerPin for 400 ms
     delay(500); // wait for 500 ms between subsequent sounds
  }
  delay(2000); // wait for 2 seconds before the next loop
}
