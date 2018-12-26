   /*     Arduino Color Sensing Tutorial
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
 #include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x3F);  // set the LCD address to 0x27 or as appropriate for a 16 chars and 2 line display
int show;
#define S0 3
#define S1 5
#define S2 6
#define S3 10
#define sensorOut 9
//#define junk 8
int frequency1,frequency2,frequency3 = 0;
int duration = 0;
void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Setting frequency-scaling to 20 %
  digitalWrite(S0,HIGH);
  
   digitalWrite(S1,LOW);
  
  Serial.begin(9600);Serial.println(digitalRead(S0));Serial.println(digitalRead(S1));
  Serial.println("LCD...");
 int error;int show;
  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Wire.beginTransmission(0x3F);
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");

  } else {
    Serial.println(": LCD not found.");
  } // if

  lcd.begin(16, 2); // initialize the lcd
  show = 0;
  test();
}

void loop() {
  // Setting red filtered photodiodes to be read
 digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  //Serial.println(digitalRead(S2));Serial.println(digitalRead(S3));
  // Reading the output frequency
  frequency1 = pulseIn(sensorOut, LOW);Serial.print("fR= ");Serial.print(frequency1);//printing freq
 // duration = pulseIn(sensorOut, LOW);Serial.print("dR= ");Serial.print(duration);//printing freq

  //Remaping the value of the frequency to the RGB Model of 0 to 255
//frequency1 = map(frequency1,104,133 ,255,0);
frequency1 = map(frequency1, 98,87,255,0);
if(frequency1 >255 || frequency1 <0) {frequency1 = 0;}
  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(frequency1);//printing RED color frequency
  Serial.print("  ");
 // 
  // Setting Green filtered photodiodes to be read
digitalWrite(S2,HIGH);
digitalWrite(S3,HIGH);
  //Serial.println(digitalRead(S2));Serial.println(digitalRead(S3));
  // Reading the output frequency
  frequency2 = pulseIn(sensorOut, LOW);Serial.print("fG= ");Serial.print(frequency2);//printing freq
  //Remaping the value of the frequency to the RGB Model of 0 to 255
// frequency2 = map(frequency2,165,300,255,0);
 frequency2 = map(frequency2,132,90,255,0);
 if(frequency2 >255 || frequency2 <0) {frequency2 = 0;}
  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(frequency2);//printing green color frequency
  Serial.print("  ");
  delay(100);
 
  // Setting Blue filtered photodiodes to be read
digitalWrite(S2,LOW);  //was S2 LOW, S3 HIGH
 digitalWrite(S3,HIGH);
 // Serial.println(digitalRead(S2));Serial.println(digitalRead(S3));
  // Reading the output frequency
  frequency3 = pulseIn(sensorOut, LOW);Serial.print("fB= ");Serial.print(frequency3);//printing freq
  //Remaping the value of the frequency to the RGB Model of 0 to 255
 frequency3 = map(frequency3,83,70,255,0);
 if(frequency3 >255 || frequency3 <0) {frequency3 = 0;}
  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(frequency3);//printing RED color frequency
  
  Serial.println("  ");
  delay(100);test();
}
void test()
{
  if (show == 0) {
    lcd.setBacklight(255);
    lcd.home(); lcd.clear();
    lcd.print("Hello LCD");
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("R=");lcd.print(frequency1);
    
    lcd.setCursor(5, 0);lcd.print("G=");lcd.print(frequency2);
    lcd.setCursor(11, 0);lcd.print("B=");lcd.print(frequency3);
     lcd.setCursor(0, 1);
     if((frequency1>frequency2) && (frequency1>frequency3) ){lcd.print("Color=Red"); 
     }
    
     else {
     if((frequency2>frequency1) && (frequency2>frequency3) ){lcd.print("Color=Green"); 
  } 
    }

     if((frequency3>frequency1) && (frequency3>frequency2)) {lcd.print("Color=Blue"); 
    } 
 
  
    delay(1000);

    

  } 
}
 

  

