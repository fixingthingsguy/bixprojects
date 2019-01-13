/* 
const int motorPin3  = D0;  //
const int motorPin4  = D5; // 
const int motorPin1  = D6;  //
const int motorPin2  = D7; // 
*/ 

const int motorPin3  = 10;  //
const int motorPin4  =  11; // 
const int motorPin1  =  12;  //
const int motorPin2  =  13; // 
const int motorPin5  =  8;  //
const int motorPin6  =  9; // 
void setup(){    //color set up
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
pinMode(motorPin5, OUTPUT);
  pinMode(motorPin6, OUTPUT);
  Serial.begin(9600);
}
void loop() {
forwpink();delay(1000);stoppink();delay(100);
forwgreen();delay(400);stopgreen();delay(100);
forworange();delay(500);stoporange();delay(100); 
backpink();delay(1000);stoppink();delay(500);
backgreen();delay(100);stopgreen();delay(100);
backorange();delay(500);stoporange();delay(100);
}
void stoporange() {
  digitalWrite(motorPin5, LOW);
    digitalWrite(motorPin6, LOW);
}
void stopgreen() {
  digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
}
void stoppink() {
  digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
}
void forwpink() {
digitalWrite(motorPin1, LOW);
digitalWrite(motorPin2, HIGH);
}
void backpink(){
digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin1, HIGH);
}
void forworange(){
  digitalWrite(motorPin5, LOW);
digitalWrite(motorPin6, HIGH);
}


void forwgreen(){
  digitalWrite(motorPin3, LOW);
digitalWrite(motorPin4, HIGH);
}

void backgreen() {
 digitalWrite(motorPin4, LOW);
   digitalWrite(motorPin3, HIGH);
}
void backorange() {
 digitalWrite(motorPin6, LOW);
   digitalWrite(motorPin5, HIGH);
}

