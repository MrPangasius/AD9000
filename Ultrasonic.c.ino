#define TRIG_PIN 13  
#define ECHO_PIN 12
#define BEEP_PIN 11 // The PIN Should be PWM


void Ultra() {
  long duration;
  long distance;
  digitalWrite(TRIG_PIN, LOW);  
  delayMicroseconds(2);  
  digitalWrite(TRIG_PIN, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(TRIG_PIN, LOW); 

  duration = pulseIn(ECHO_PIN, HIGH);  

  distance = (duration*.0343)/2;

  if (distance >300 || distance < 0)
  {
    Serial.println("[Ultra Sonic Sensor] : Bad Reading\n\r");
  }
  else
  {
    Serial.print("[Ultra Sonic Sensor] : Distance : ");  
    Serial.println(distance);
    
    if (distance < 61 && distance  > 50){
      analogWrite(BEEP_PIN, 63);
    }
    if (distance < 51 && distance > 40){
      analogWrite(BEEP_PIN, 127);
    }
    if (distance < 41 && distance > 20){
      analogWrite(BEEP_PIN, 191);
    }
    if (distance < 21){
      analogWrite(BEEP_PIN, 256);
    }
  }

}
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(TRIG_PIN, OUTPUT);  
  pinMode(ECHO_PIN, INPUT);  
  pinMode(BEEP_PIN, OUTPUT);
  Serial.begin(9600); 

}
void loop() 
{
  Ultra();
}
