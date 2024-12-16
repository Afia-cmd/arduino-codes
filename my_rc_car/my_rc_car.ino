 #include<SoftwareSerial.h>
 int motor1= 9;
 int motor2= 11;
 int motor3= 6;
 int motor4= 5;
char command;

void setup() {
  // put your setup code here, to run once:
pinMode(motor1,OUTPUT);//Right forward
pinMode(motor2,OUTPUT);//Right reverse
pinMode(motor3,OUTPUT);//Left forward
pinMode(motor4,OUTPUT);//Left reverse

Serial.begin(38400);
}

void loop() {
  // put your main code here, to run repeatedly:

if (Serial.available()>0){
  command=Serial.read();
}

if (command =='F'){
    digitalWrite(motor1,HIGH);
    digitalWrite(motor3,HIGH);
  }
  
else if(command=='B'){
  digitalWrite(motor2, HIGH);
  digitalWrite(motor4,HIGH);
}

else if(command=='L'){
  digitalWrite (motor1,HIGH);
}

  else if(command=='R'){
  digitalWrite(motor3,HIGH);
}

else (command=='S');{
  digitalWrite(motor1,LOW);
  digitalWrite(motor2,LOW);
  digitalWrite(motor3,LOW);
  digitalWrite(motor4,LOW);
}
delay(100);
}
