 /*     Arduino Color Sensing Tutorial
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
 
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

int frequency1 = 0;int R,G,B;
int frequency2 = 0;
int frequency3 = 0;
//L293D
/* from various sources
 *  
 */
const int motorPin1  = 49;  // Pin 14 of L293
const int motorPin2  = 48; // Pin 10 of L293
const int motorPin3  = 51;// Pin 14 of L293
const int motorPin4  = 50;// Pin 10 of L293
const int motorPin5  = 52;// Pin 14 of L293
const int motorPin6  = 53;//Pin 10 of L293
 int val;
//This will run only one time.
void setup(){    //color set up
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);      

                  //color set up
 Serial.begin(9600);pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);pinMode(motorPin2, OUTPUT);pinMode(motorPin1, OUTPUT);pinMode(motorPin5, OUTPUT);pinMode(motorPin6, OUTPUT);
 int val;
/*
//This code will turn Motor  clockwise for 2 sec to reach stop point.
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
//Stop
  delay(800);  Serial.println(analogRead(A10)); 
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);Serial.println(analogRead(A10)); 
  delay(2000); 
  //This code will turn Motor  return for 2 sec to reach stop point.
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
//Stop
  delay(800);  Serial.println(analogRead(A10)); 
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);Serial.println(analogRead(A10)); 
  delay(2000);
*/
 
  //grabber open  
// digitalWrite(motorPin4,HIGH);digitalWrite(motorPin3,LOW);delay(2000);digitalWrite(motorPin3,LOW);digitalWrite(motorPin4,LOW); delay(500);
 //grabber close
// digitalWrite(motorPin4,LOW);digitalWrite(motorPin3,HIGH);delay(2000);digitalWrite(motorPin3,LOW);digitalWrite(motorPin4,LOW); delay(3000);
 //dipper goes UP here   TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT




  Calibrate();

}

void TimedCCW(int);
void MeasureRGB();

void Calibrate()
{
  GrabberClose();
  GrabberOpen();
  GrabberClose();
  GrabberOpen();
  GrabberClose();
  
  TimedCCW(1000);
  int done = 0;
  while(!done)
  {
    MeasureRGB();
    if(R > G + 50 && R > B + 50)
    {
      break;
    }

    TimedCCW(10);
  }
}


void loop(){
 retfrompos2();  //This code will turn BASE  clockwise for 2 sec to reach START point from Pos2 ONLY
//dipper goes DOWN here
// digitalWrite(motorPin6,HIGH);digitalWrite(motorPin5,LOW);delay(50);
// digitalWrite(motorPin6,LOW);digitalWrite(motorPin5,LOW); delay(2000); 
armdown();
 
 int i=0;
 R=0;G=0;B=0;
 while(i<20){color();//Serial.print(frequency1);Serial.print(frequency3);Serial.print(frequency3);
 R=R+frequency1;G=G+frequency2;B=B+frequency3; i++;
 }Serial.print("R= ");Serial.print(R/i);Serial.print("G=  ");Serial.print(G/i);Serial.print("B=  ");Serial.println(B/i);
 
 //grabber close
  digitalWrite(motorPin4,LOW);digitalWrite(motorPin3,HIGH);delay(5000);digitalWrite(motorPin3,LOW);digitalWrite(motorPin4,LOW); delay(3000); 
   //dipper goes UP here   TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
 delay(500); Serial.print("Dipper Position"); //Serial.println(analogRead(A2));
//digitalWrite(motorPin6,LOW);Serial.println(motorPin6);digitalWrite(motorPin5,HIGH);Serial.println(motorPin5);delay(300);
//digitalWrite(motorPin6,LOW);Serial.println(motorPin6);digitalWrite(motorPin5,LOW); Serial.println(motorPin5);delay(2000); 
armup();
//which color is it?

  pos1();if((R>G)&& (R>B)){Serial.println("its Red");
          armdown();
         //grabber open  
          digitalWrite(motorPin4,HIGH);digitalWrite(motorPin3,LOW);delay(2000);digitalWrite(motorPin3,LOW);digitalWrite(motorPin4,LOW); delay(500);
          armup();
  }
  pos2();if((G>R)&&(G>B)){Serial.println("its Green");
         armdown();
         //grabber open  
          digitalWrite(motorPin4,HIGH);digitalWrite(motorPin3,LOW);delay(2000);digitalWrite(motorPin3,LOW);digitalWrite(motorPin4,LOW); delay(500);
         armup();
  }
  pos3();if((B>R) && (B>G)){Serial.println("its Blue");
         armdown();
         //grabber open  
          digitalWrite(motorPin4,HIGH);digitalWrite(motorPin3,LOW);delay(2000);digitalWrite(motorPin3,LOW);digitalWrite(motorPin4,LOW); delay(500);
         armup();
  }
  
  delay(2000);
}

void StartCCW()
{
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  Serial.println("CCW");
}

void StartCW()
{
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
}

void Stop()
{
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
}

void TimedCCW(int z)
{
  StartCCW();
  delay(z);
  Stop();
}

void TimedCW(int z)
{
  StartCW();
  delay(z);
  Stop();
}

void MeasureRGB()
{
   R=0;G=0;B=0;
   int i = 0;
 while(i<20){color();//Serial.print(frequency1);Serial.print(frequency3);Serial.print(frequency3);
 R=R+frequency1;G=G+frequency2;B=B+frequency3; i++;
 }Serial.print("R= ");Serial.print(R/i);Serial.print("G=  ");Serial.print(G/i);Serial.print("B=  ");Serial.println(B/i);

 R = R / i;
 G = G / i;
 B = B / i;
}

void GrabberClose()
{
  digitalWrite(motorPin4,LOW);
  digitalWrite(motorPin3,HIGH);delay(2000);
  digitalWrite(motorPin3,LOW);
  digitalWrite(motorPin4,LOW);
}

void GrabberOpen()
{
  digitalWrite(motorPin4,HIGH);
  digitalWrite(motorPin3,LOW);delay(2000);
  digitalWrite(motorPin3,LOW);
  digitalWrite(motorPin4,LOW);
}

void pos1(){
  //This code will turn BASE counter-clockwise  to Pos 1
    digitalWrite(motorPin1, LOW);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    digitalWrite(motorPin2, HIGH);
    delay(500);Serial.println(analogRead(A10)); 
  //And this code will STOP motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    delay(5000);Serial.println(analogRead(A10));   

   //dipper goes UP here
/* 
  //delay(500); Serial.print("Dipper Position"); Serial.println(analogRead(A2));
digitalWrite(motorPin5,HIGH);digitalWrite(motorPin6,LOW);delay(1000);
 digitalWrite(motorPin6,LOW);digitalWrite(motorPin5,LOW); delay(500);
 */
}

void pos2() {
 //This code will turn BASE counter-clockwise to Pos 2 
  digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    delay(500);Serial.println(analogRead(A10)); 
  //And this code will STOP motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    delay(5000);Serial.println(analogRead(A10)); 

}
void pos3() {
  //This code will turn BASE counter-clockwise to Pos 3 
  digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    delay(500);Serial.println(analogRead(A10)); 
  //And this code will STOP motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    delay(5000);Serial.println(analogRead(A10)); 
}
void retfrompos2() {
  //This code will turn BASE  clockwise for 2 sec to reach START point from Pos3 ONLY
   digitalWrite(motorPin1, HIGH);
   digitalWrite(motorPin2, LOW);
   delay(2000);  Serial.println(analogRead(A10));
  //stop
   digitalWrite(motorPin1, LOW);
   digitalWrite(motorPin2, LOW);Serial.println(analogRead(A10)); 
   delay(2000);//Serial.println(analogRead(A10));
  //And this code will STOP motors
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    delay(5000); // Serial.println(analogRead(A10));
     //grabber open  
// digitalWrite(motorPin4,HIGH);digitalWrite(motorPin3,LOW);delay(2000);digitalWrite(motorPin3,LOW);digitalWrite(motorPin4,LOW); delay(500);

}
void color(){
   // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  frequency1 = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
frequency1 = map(frequency1,104,119,255,0);
  // Printing the value on the serial monitor
 // Serial.print("R= ");//printing name
 // Serial.print(frequency1);//printing RED color frequency
 // Serial.print("  ");
  delay(100);

  // Setting Green filtered photodiodes to be read
digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  frequency2 = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
 frequency2 = map(frequency2,165,300,255,0);
  // Printing the value on the serial monitor
  //Serial.print("G= ");//printing name
  //Serial.print(frequency2);//printing RED color frequency
  //Serial.print("  ");
  delay(100);
 
  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,HIGH);  //was S2 LOW, S3 HIGH
  digitalWrite(S3,LOW);
  // Reading the output frequency
  frequency3 = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
 frequency3 = map(frequency3,109,220,255,0);
  // Printing the value on the serial monitor
  //Serial.print("B= ");//printing name
  //Serial.print(frequency3);//printing RED color frequency
  
  //Serial.println("  ");
  delay(100);
}
void armup(){
   delay(500); Serial.print("Dipper Position"); //Serial.println(analogRead(A2));
digitalWrite(motorPin6,LOW);Serial.println(digitalRead(motorPin6));digitalWrite(motorPin5,HIGH);Serial.println(digitalRead(motorPin5));delay(250);
digitalWrite(motorPin6,LOW);Serial.println(digitalRead(motorPin6));digitalWrite(motorPin5,LOW); Serial.println(digitalRead(motorPin5));delay(50); 
}
void armdown(){
digitalWrite(motorPin6,HIGH);Serial.println(digitalRead(motorPin6));digitalWrite(motorPin5,LOW);Serial.println(digitalRead(motorPin5));delay(50);
 digitalWrite(motorPin6,LOW);Serial.println(digitalRead(motorPin6));digitalWrite(motorPin5,LOW);Serial.println(digitalRead(motorPin5)); delay(50);   
}
