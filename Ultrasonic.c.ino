const int trigPin = 9;  
const int echoPin = 10;
int duration = 0;
int distance = 0;

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin, INPUT);  
  Serial.begin(9600); 

}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2);  
  digitalWrite(trigPin, HIGH);  
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW); 

  duration = pulseIn(echoPin, HIGH);  

  distance = (duration*.0343)/2;
  if (distance < 0 or distance > 300)
  {
    Serial.print("Bad Reading...");
  }
  else{
    Serial.print("Distance: ");  
    Serial.println(distance); 
  }
  delay(1000);                       // wait for a second
}
