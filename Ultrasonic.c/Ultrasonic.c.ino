const int trigPin = 13;  
const int echoPin = 12;
const int BeepPin = 11;
long duration = 0;
long distance = 0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);  
  pinMode(BeepPin, OUTPUT);
  Serial.begin(9600); 

}
void loop() {
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2);  
  digitalWrite(trigPin, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW); 

  duration = pulseIn(echoPin, HIGH);  

  distance = (duration*.0343)/2;
  if (distance < 20){
      digitalWrite(BeepPin, HIGH);
    }
    Serial.print("Distance: ");  
    Serial.println(distance); 
  delay(1000);
  digitalWrite(BeepPin, LOW);
}
