#define trigPin D1
#define echoPin D4
int relay = D2;  
int relay2 = D1;
#define switchPin D0
bool flag = false;

void setup() {
  Serial.begin (921600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  digitalWrite(relay, LOW);
  digitalWrite(relay2, LOW);
}

void loop() {
  Serial.println(switchPin);
  long duration, distance;
  digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
  //Serial.println(distance);

  if(digitalRead(switchPin) == LOW)
  {
    if(flag)
    {
      flag = false;
      }else
      {
        flag = true;
        }
    }
    
    if(flag)
    {
          digitalWrite(relay, HIGH);
          digitalWrite(relay2, LOW);
      }else
      {
        digitalWrite(relay, LOW);
        digitalWrite(relay2, HIGH);
        }

  delay(50);
}
