
// Use if you want to force the software SPI subsystem to be used for some     reason (generally, you don't)
// #define FASTLED_FORCE_SOFTWARE_SPI
// Use if you want to force non-accelerated pin access (hint: you really don't, it breaks lots of things)
// #define FASTLED_FORCE_SOFTWARE_SPI
// #define FASTLED_FORCE_SOFTWARE_PINS
#include "FastLED.h"

///////////////////////////////////////////////////////////////////////////////////////////
//
// Move a white dot along the strip of leds.  This program simply shows how to configure the leds,
// and then how to turn a single pixel white and then off, moving down the line of pixels.
// 

// How many leds are in the strip?
#define NUM_LEDS 12

// Data pin that led data will be written out over
#define DATA_PIN 3

// Clock pin only needed for SPI based chipsets when not using hardware SPI
//#define CLOCK_PIN 8

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

void setup() {
    // sanity check delay - allows reprogramming if accidently blowing power w/leds
delay(2000);
FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUM_LEDS);
  leds[0] = CRGB::Red;
 // leds[8] = CRGB::Red;
 // leds[15] = CRGB::Red;
 // leds[18]=CRGB::Blue;
  FastLED.show();
}

// your leds.
void loop() {/*
  leds[0] = CRGB::Red;
  leds[8] = CRGB::Red;
  leds[15] = CRGB::Red;
  FastLED.show();
*/
}
