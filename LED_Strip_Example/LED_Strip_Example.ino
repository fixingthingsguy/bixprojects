//
// Copyright (c) ASB 2019
//
// Example code for running the WS2812b LED strip.
//
// To use:
//
//    * Define NUM_LEDS below to the number of LEDS in your strip.  A 16' strip has 150.  You can find out
//      exactly how many LEDS you have by picking a number, and then using the TurnOnLED API to find your last LED
//      (increasing or decreasing NUM_LEDs until you find the right number).
//
//    * Plug the WS28128B data line into pin 7.  If other pins are needed, I can change
//      to any other pin on port D fairly easily.
//
//    * Change the 'delayms' member variable in the Display class to make the updates faster or slower.
//
// The code in loop() runs a simple rainbow effect and also demonstrates turning on/off a few LEDs.
//



#define STR(x) #x
#define XSTR(s) STR(s)

#define NUM_LEDS 150
#define NUM_BYTES (NUM_LEDS*3)
volatile unsigned char data[NUM_BYTES];

void setup() {
  // put your setup code here, to run once:

#if 1
  // Set pin7 to write
  __asm__("in r24, 0x0a");
  __asm__("ori r24, 0x80");
  __asm__("out 0x0a, r24");


#endif

  Serial.begin(9600);
  Serial.print("F_CPU = ");
  Serial.println(F_CPU);
}

#define DELAY_ONE() __asm__("lsl r20");
volatile unsigned char ccc = 0;           
void RenderLEDs()
{
  // R28:R29 = byte counter (0 - ...)
  // R19 = bit counter (0-8)
  // R24 = data
  // R20 = 0
  // R21 = 1
  ccc = 0;
  __asm__("out 0x0b, r20");
  delay(1);

  // INITIALIZATION
  __asm__("ldi r28, " XSTR(NUM_BYTES&0xFF));
  __asm__("ldi r29, " XSTR((NUM_BYTES&0xFF00)>>8));
  __asm__("ldi r20, 0");
  __asm__("ldi r21, 0x80");
  __asm__("cli");

  __asm__("ldi r30, lo8(data)");
  __asm__("ldi r31, hi8(data)");

  // DATA LOOP TOP
  __asm__("7:mov r25, r28");          // 1
  __asm__("or r25, r29");
  __asm__("breq  6f");              // 1-2
  __asm__("ldi r19, 8");            // 1
  __asm__("ld r24, Z+");            // 1

  __asm__("sts ccc, r24");
 
  // TOP OF BYTE LOOP
  __asm__("1: sbrc r24, 0");        // 1
  __asm__("   rjmp 4f");            // 2

                                    // TOTAL: 7
  // SEND ZERO CODE
  // T0H
  __asm__("3: out 0x0b, r21");       // 1
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  
  // T0L
  __asm__("out 0x0b, r20");       // 1
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
   
  __asm__("rjmp 5f");         // 2

  // SEND ONE
  // T1H
  __asm__("4: out 0x0b, r21");
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();
  DELAY_ONE();

  // T1L
  __asm__("out 0x0b, r20");


  __asm__("5:lsr r24");         // 1
  __asm__("dec r19");         // 1
  __asm__("brne 1b");         // 1-2

  __asm__("sbiw r28, 1");         // 1
  __asm__("rjmp 7b");         // 2
                              // TOTAL: 6

  // RETURN
  __asm__("6: nop");
  __asm__("sei");
}
unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

struct Display
{
  int counter;
  int delayms;

  Display() : counter(0), delayms(3)
  {
    
  }

  unsigned char sat(int val)
  {
    if(val < 0)
      return 0;
    if(val > 255)
      return 255;
    return val;
  }

  unsigned char scale256(int val, int maxval)
  {
    float newVal = 256.0f * val / maxval;
    int iNewVal = static_cast<int>(newVal);
    return sat(iNewVal);
  }

  void Update()
  {
    for(int i = 0; i < NUM_BYTES; ++i)
    {
      data[i] = reverse(data[i]);
    }

    RenderLEDs();

    for(int i = 0; i < NUM_BYTES; ++i)
    {
      data[i] = 0;
    }
    
    delay(delayms);
    counter += 1;
  }

  void DoRainbow()
  {
    const int CYCLES = 1200;
    const int CYCLES_COLOR = CYCLES/3;
    counter = counter % CYCLES;
    
    int cyclenum = counter % CYCLES;
    int G = 0;
    int R = 0;
    int B = 0;

    if(cyclenum < CYCLES_COLOR)
    {
        G = cyclenum;
        R = (CYCLES_COLOR - cyclenum);
    }
    else if(cyclenum < 2*CYCLES_COLOR)
    {
      cyclenum -= CYCLES_COLOR;

        B = cyclenum;
        G = (CYCLES_COLOR - cyclenum);
    }
    else
    {
      cyclenum -= 2*CYCLES_COLOR;
      R = cyclenum;
      B = (CYCLES_COLOR - cyclenum);
    }

    G = scale256(G, CYCLES_COLOR);
    R = scale256(R, CYCLES_COLOR);
    B = scale256(B, CYCLES_COLOR);

   
    for(int i = 0; i < NUM_LEDS; ++i)
    {
      int idx = i*3;
      data[idx + 0] = G;
      data[idx + 1] = R;
      data[idx + 2] = B;
    }
  }

  // Turns on a single LED.  Brightness must be 0-255.
  void TurnOnLED(int num, int brightness)
  {
    data[num*3 + 0] = sat(brightness);
    data[num*3 + 1] = sat(brightness);
    data[num*3 + 2] = sat(brightness);
  }

  void TurnOffLED(int num)
  {
    data[num * 3 + 0] = 0;
    data[num * 3 + 1] = 0;
    data[num * 3 + 2] = 0;
  }
};

Display disp;

void loop() {
  disp.DoRainbow();
  disp.TurnOnLED(30, 128);
  disp.TurnOffLED(31);
  disp.TurnOffLED(29);
  disp.Update();
}
