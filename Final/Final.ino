#include <Wire.h>
#include <SoftwareSerial.h>

#define TRIG_PIN 24
#define ECHO_PIN 25
#define BEEP_PIN 3


void setup() {
  pinMode(TRIG_PIN, OUTPUT);  
  pinMode(ECHO_PIN, INPUT);  
  pinMode(BEEP_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  Ultra();
  delay(1000);
}

void Ultra()
{
  unsigned long duration;
  unsigned long distance;
  
  digitalWrite(TRIG_PIN, LOW);  
  delayMicroseconds(2);  
  digitalWrite(TRIG_PIN, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(TRIG_PIN, LOW); 

  duration = pulseIn(ECHO_PIN, HIGH);  

  distance = (duration*.0343)/2;
  if (distance > 300)
  {
    Serial.println(bbd reading in Ultra Sonic Sensor");
  }
  else
  {
    Serial.print("Distance: ");  
    Serial.println(distance); 
  }
  
  if (distance < 20)
  {
    digitalWrite(BEEP_PIN, HIGH);
  }
  delay(1000);
  digitalWrite(BEEP_PIN, LOW);

}
