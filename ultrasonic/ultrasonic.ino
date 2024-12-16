const int trigPin =10;
const int echoPin=9;
float duration;
float distance;

void setup() {
  // put your setup code here, to run once:
pinMode(trigPin,OUTPUT);
pinMode(echoPin,INPUT);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(trigPin,LOW);
delayMicroseconds(2);
digitalWrite(trigPin,HIGH);
delayMicroseconds(10);
digitalWrite(trigPin,LOW);

duration=pulseIn(echoPin,HIGH);
distance=(0.0343*duration)/2;
Serial.print("distance:");
Serial.println(distance);
delay(2000);
}
