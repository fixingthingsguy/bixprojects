   /*     Arduino Color Sensing Tutorial
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
#include <avr/pgmspace.h> 
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

LiquidCrystal_PCF8574 lcd(0x3F);  // set the LCD address to 0x27 or as appropriate for a 16 chars and 2 line display
int show;
#define S0 3
#define S1 5
#define S2 6
#define S3 10
#define sensorOut 9

int frequency1,frequency2,frequency3 = 0;
int duration = 0;

enum Color
{
  Blue = 0,
  Red = 1,
  Green = 2,
  Purple = 3,
  Orange = 4,
  Yellow = 5
};

char *ColorToString(Color c)
{
  switch(c)
  {
    case Blue: return ("Blue");
    case Red: return ("Red");
    case Green: return ("Green");
    case Purple: return ("Purple");
    case Orange: return ("Orange");
    case Yellow: return ("Yellow");
  }

  return ("Unknown");
}

const PROGMEM long data  [][6] = {
/*  Color, SampleNumber, R, G, B, W   */
  { Blue, 0, 16025, 24271, 53763, 94339 },
  { Blue, 0, 17361, 26315, 58823, 102040 },
  { Blue, 0, 17482, 26737, 60240, 104166 },
  { Blue, 0, 17793, 27173, 60975, 104166 },
  { Blue, 1, 20000, 33783, 84745, 138888 },
  { Blue, 1, 20080, 33783, 84745, 138888 },
  { Blue, 1, 20080, 33783, 84745, 138888 },
  { Blue, 1, 20080, 33783, 84745, 138888 },
  { Blue, 2, 20491, 36231, 92592, 156250 },
  { Blue, 3, 20080, 30487, 69444, 119047 },
  { Blue, 3, 20920, 31847, 72463, 125000 },
  { Blue, 3, 21186, 32258, 72463, 121951 },
  { Red, 0, 63291, 27624, 53191, 125000 },
  { Red, 0, 51020, 27322, 53191, 125000 },
  { Red, 0, 51020, 27322, 53191, 125000 },
  { Red, 0, 51020, 27322, 53191, 125000 },
  { Red, 1, 42735, 24875, 45045, 102040 },
  { Red, 1, 41666, 23255, 43859, 104166 },
  { Red, 1, 46296, 24752, 47169, 113636 },
  { Red, 1, 47169, 24509, 46296, 111111 },
  { Red, 2, 54347, 26455, 50000, 121951 },
  { Red, 2, 51020, 25641, 48543, 119047 },
  { Red, 2, 53191, 25906, 49019, 119047 },
  { Red, 3, 50000, 20746, 31847, 64935 },
  { Green, 0, 22421, 36496, 47169, 106382 },
  { Green, 0, 23474, 37593, 48076, 108695 },
  { Green, 2, 24154, 40322, 54347, 135135 },
  { Green, 3, 28735, 49019, 60240, 138888 },
  { Green, 3, 28571, 48543, 60240, 138888 },
  { Green, 3, 28571, 48543, 60240, 138888 },
  { Green, 4, 31446, 50000, 57471, 135135 },
  { Green, 5, 28735, 46296, 55555, 131578 },
  { Green, 5, 29761, 48076, 56818, 135135 },
  { Green, 5, 30674, 47169, 49504, 90909 },
  { Purple, 0, 45045, 40983, 81967, 151515 },
  { Purple, 1, 30303, 29940, 62500, 119047 },
  { Purple, 1, 31645, 30864, 64102, 125000 },
  { Purple, 2, 43103, 41666, 94339, 172413 },
  { Purple, 2, 42016, 41666, 94339, 172413 },
  { Purple, 2, 42016, 41666, 96153, 172413 },
  { Purple, 2, 42016, 41666, 94339, 172413 },
  { Purple, 3, 51546, 44642, 92592, 178571 },
  { Purple, 3, 50505, 45045, 98039, 185185 },
  { Purple, 3, 52631, 47169, 102040, 192307 },
  { Orange, 0, 60240, 29761, 47169, 131578 },
  { Orange, 0, 59523, 29411, 46728, 131578 },
  { Orange, 0, 60240, 29761, 46728, 131578 },
  { Orange, 0, 60240, 30487, 50000, 135135 },
  { Orange, 0, 60975, 30674, 50000, 135135 },
  { Orange, 0, 60975, 30674, 50000, 135135 },
  { Orange, 1, 75757, 34482, 53763, 156250 },
  { Orange, 2, 87719, 38167, 58139, 172413 },
  { Orange, 2, 87719, 37593, 57471, 172413 },
  { Yellow, 0, 74626, 59523, 62500, 192307 },
  { Yellow, 0, 72463, 57471, 60975, 185185 },
  { Yellow, 1, 81967, 62500, 64935, 200000 },
  { Yellow, 2, 78125, 63291, 67567, 227272 },
  { Yellow, 2, 78125, 63291, 68493, 208333 },
  { Yellow, 2, 78125, 63291, 68493, 208333 },
  { Yellow, 2, 78125, 63291, 68493, 208333 },
  { Yellow, 2, 79365, 64935, 71428, 227272 },
  { Yellow, 3, 96153, 72463, 73529, 238095 },
  { Yellow, 3, 98039, 73529, 73529, 238095 },
  { Yellow, 3, 100000, 75757, 74626, 250000 },
};

// Reads data from the data[] array.  Requires usage of the
// special functions to read from flash memory - normal C memory
// accesses do not work properly.
#define DATA_FIELD_COLOR 0
#define DATA_FIELD_SAMPLE 1
#define DATA_FIELD_R 2
#define DATA_FIELD_G 3
#define DATA_FIELD_B 4
#define DATA_FIELD_W 5
long ReadFlashData(int row, int field)
{
  long addr = (long)pgm_get_far_address(data);
  addr += field * sizeof(long);
  addr += sizeof(data[0])*row;
    
  long tmp;
  memcpy_PF(&tmp, addr, sizeof(long));

  return tmp;
}


void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
  Serial.begin(9600);
  Serial.println(digitalRead(S0));
  Serial.println(digitalRead(S1));
  Serial.println("LCD...");
  
  int error;
  while (! Serial);

  Serial.println("Dose: check for LCD");

  // See http://playground.arduino.cc/Main/I2cScanner
  Wire.begin();
  Wire.beginTransmission(0x3F);
  error = Wire.endTransmission();
  Serial.print("I2C result: ");
  Serial.print(error);

  if (error == 0) {
    Serial.println(": LCD found.");

  } else {
    Serial.println(": LCD not found.");
  } // if

  lcd.begin(16, 2); // initialize the lcd

  lcd.setBacklight(255);
  lcd.clear();
  lcd.home();
  lcd.setCursor(0, 0); 
  lcd.print("Initialized");
  delay(500);
}

const int RED=1;
const int BLUE=2;
const int GREEN=3;
const int WHITE=4;

unsigned long ReadPeriod(int color)
{
  const int iter = 250;
  
  switch(color)
  {
    case RED:
      digitalWrite(S2,LOW);
      digitalWrite(S3,LOW);
      break;

    case GREEN:
      digitalWrite(S2,HIGH);
      digitalWrite(S3,HIGH);
      break;

    case BLUE:
      digitalWrite(S2,LOW);
      digitalWrite(S3,HIGH);
      break;

    case WHITE:
      digitalWrite(S2,HIGH);
      digitalWrite(S3,LOW);
      break;
    
  }
  unsigned long sum = 0;
  for(int i = 0; i < iter; ++i)
  {
    sum += pulseIn(sensorOut, LOW);
  }

  sum = sum / iter;

  unsigned long hz = 5000000 / sum;

  return hz;
}

Color GetBestColor(long R, long G, long B, long W)
{
  Color best;
  float bestVal = 1e20f;
  float curr = 1e20f;

  for (int i = 0; i < sizeof(data) / sizeof(data[0]); ++i)
  {
    if (i > 0 && ReadFlashData(i, DATA_FIELD_SAMPLE) != ReadFlashData(i-1, DATA_FIELD_SAMPLE))
    {
      //printf("Color %d, sample %d, val = %0.2f\n", data[i - 1][0], data[i-1][1], curr);
      if (curr < bestVal)
      {
        bestVal = curr;
        best = ReadFlashData(i - 1, DATA_FIELD_COLOR);
        //printf("New best: color %d [val = %0.2f]\n", best, bestVal);
        //Serial.print("New best: color ");
        //Serial.print(best);
        //Serial.print(", dist = ");
        //Serial.println(bestVal > 1000000 ? 999999 : bestVal);
      }
      curr = 1e20f;
    }
/*
  Serial.print(R);
  Serial.print("; ");
  Serial.print(data[i].R);
  Serial.print("; ");
  Serial.println(R - data[i].R);
 */
    
    float distSquared = pow(R - ReadFlashData(i, DATA_FIELD_R), 2) 
      + pow(G - ReadFlashData(i, DATA_FIELD_G), 2)
      + pow(B - ReadFlashData(i, DATA_FIELD_B), 2) 
      + pow(W - ReadFlashData(i, DATA_FIELD_W), 2);

      //Serial.println(distSquared);

    float f = sqrt(distSquared);
      
    if (f < curr)
    {
      curr = f;
    }
  }

  return best;
}

void loop() 
{
  long R = ReadPeriod(RED);
  long G = ReadPeriod(GREEN);
  long B = ReadPeriod(BLUE);
  long W = ReadPeriod(WHITE);

  Serial.print("R=");
  Serial.print(R);
  Serial.print("; ");

  Serial.print("G=");
  Serial.print(G);
  Serial.print("; ");

  Serial.print("B=");
  Serial.print(B);
  Serial.print("; ");

  Serial.print("W=");
  Serial.print(W);
  Serial.print("; ");

  Serial.println();


  Color best = GetBestColor(R, G, B, W);

  Serial.print("Best match: ");
  Serial.println(best);
  Serial.println(ColorToString(best));
  

  lcd.clear();
  lcd.print("Best match:");
  lcd.setCursor(0, 1);
  lcd.print(ColorToString(best));
  

  delay(300);
  
}

  
