// This is a PARED-DOWN NeoPixel example for the Adafruit_NeoPixel_ZeroDMA
// library, demonstrating pin declarations, etc.  For more complete examples
// of NeoPixel operations, see the examples included with the 'regular'
// Adafruit_NeoPixel library.

// Also requires LATEST Adafruit_NeoPixel and Adafruit_ZeroDMA libraries.

#include <Adafruit_NeoPixel_ZeroDMA.h>

// DMA NeoPixels work ONLY on SPECIFIC PINS.
// On Circuit Playground Express: 8, A2 and A7 (TX) are valid.
// On Feather M0, Arduino Zero, etc.: 5, 11, A5 and 23 (SPI MOSI).
// On GEMMA M0: pin 0.
// On Trinket M0: pin 4.
// On Metro M4: 3, 6, 8, 11, A3 and MOSI
#define PIN         4
#define NUM_PIXELS 56


const int cometSpeed = 10;
const int cometColourCount = 1;
const int cometSegmentSize = 3;
uint32_t cometColourValues[1] = { 0x808080 };
const uint32_t randomColourValues[7] = { 0x808080, 0x800000, 0x008000, 0x000080, 0x808000, 0x008080, 0x800080 };

const int stripeSpeed = 1000;

byte switchValLast;

Adafruit_NeoPixel_ZeroDMA strip(NUM_PIXELS, PIN, NEO_GRB);

const int halfPixels = NUM_PIXELS/2;
byte animNum = 1;

int cometPos = 0;
unsigned long lastSwitch;
unsigned long lastComet;
unsigned long lastStripe;

void setup() {  
  pinMode(0, INPUT_PULLDOWN);
  
  strip.begin();
  strip.setBrightness(32);
  strip.show();

  randomSeed(A0);
}

void loop() {
  unsigned long now = millis();
  uint32_t t = micros();

  byte switchVal = digitalRead(0);

  if (switchVal != switchValLast && now-lastSwitch>100) {
    animNum++;
    if (animNum > 3) animNum = 0;
    lastSwitch = now;
  }

  if (animNum == 1) {
    // Rainbow cycle
    for(uint16_t i=0; i<=halfPixels; i++) {
      uint32_t colour =  Wheel((uint8_t)(
        (t * 256 / 1048576) + i * 256 / strip.numPixels()));
      strip.setPixelColor(i,colour);
      strip.setPixelColor(strip.numPixels()-i-1,colour);
    }
  } else if (animNum == 2) {
    // Draw comet
    if (now - lastComet > cometSpeed) {
      drawComet(halfPixels+cometPos);
      cometPos++;
      if (cometPos >= halfPixels) {
        cometPos = 0;
        cometColourValues[0] = randomColourValues[random(7)];
      }
      for(uint16_t i=0; i<halfPixels; i++) {
        strip.setPixelColor(i,strip.getPixelColor(strip.numPixels()-i-1));
      }
      lastComet = now;
    }
  } else if (animNum == 3) {
    // Draw stripes
    if (now - lastStripe > stripeSpeed) {
      for(uint16_t i=halfPixels; i<strip.numPixels(); i++) {
        if (i%2 == 1) {
          strip.setPixelColor(i,randomColourValues[random(7)]);
        } else {
          strip.setPixelColor(i,0x000000);
        }
      }
      for(uint16_t i=0; i<halfPixels; i++) {
        strip.setPixelColor(i,strip.getPixelColor(strip.numPixels()-i-1));
      }
      lastStripe = now;
    }
  } else {
    strip.clear();
  }
  strip.show();

  switchValLast = switchVal;
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(uint8_t WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void drawComet(int startPos) {
  if (startPos < 0 || startPos >= strip.numPixels()) startPos = 0;

  // Clear the strip
  strip.clear();

  for (int c = 0; c < cometColourCount; c++) {
    for (int s = 0; s < cometSegmentSize; s++) {
      strip.setPixelColor(startPos, cometColourValues[c]);
      if (startPos == 0) startPos = strip.numPixels();
      startPos--;
    }
  }

  // Display the animation
  strip.show();

  return;
}
