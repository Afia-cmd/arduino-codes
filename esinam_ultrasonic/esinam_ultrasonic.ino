const int trig=9;
const int echoe=10;
float duration,distance;
float speed = 0.0343;

void setup() {
  pinMode(trig,OUTPUT);
  pinMode(echoe, INPUT);
  Serial.begin(9600);
  // put your setup code here, to run once:

}

void loop() {
  digitalWrite(trig,LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig,LOW);

  duration= pulseIn(echoe, HIGH);
  distance = (speed * duration)/2;
  Serial.print("distance:");
  Serial.println(distance);
  delay(2000);
}

  // put your main code here, to run repeatedly:}
