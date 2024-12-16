const int trig=9;
const int echo=10;
const int buzzer=11;
float distance;
float duration;
void setup() {
  // put your setup code here, to run once:
pinMode(trig,OUTPUT);
pinMode(echo, INPUT);
pinMode(buzzer,OUTPUT);
digitalWrite(buzzer,LOW);
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
digitalWrite(trig,LOW);
delayMicroseconds(2);
digitalWrite(trig,HIGH);
delayMicroseconds(10);
digitalWrite(trig,LOW);


duration=pulseIn(echo,HIGH);
distance=(0.0343*duration)/2;

if((distance<=0) || (distance>=30)){
  digitalWrite(buzzer,LOW);
  Serial.print("distance:CANT BE DETERMINED");
Serial.println();
}
else{
  digitalWrite(buzzer, HIGH);
  Serial.print("distance:");
  Serial.println(distance);
delay(500);
}
}