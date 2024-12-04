#include <FastLED.h>

#define LED_PIN MOSI
//#define LED_PIN_2 A1
#define NUM_LEDS 117
#define MAXBRIGHTNESS 255
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

CRGBPalette16 gPal;
#define COOLING 55 //55
#define FRAMES_PER_SECOND 60

int LEDdecayMs = 100;
int decreaseIncrement = 1;

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 200

int currentBrightness = MAXBRIGHTNESS;

CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 PinkPalette PROGMEM;
extern const TProgmemPalette16 YellowPalette PROGMEM;
//int sensorPin = A11;  // select the input pin for the potentiometer

bool gReverseDirection = false;

unsigned long previousMillis_changeMode = 0;  // will store last time LED was updated
unsigned long previousMillis_runLEDs = 0;
int intervalFPSled = 10;

int animationMode = 0;

bool triggerLEDflag = true;

unsigned long stampMill_triggerLED = 0;
#include "fx/1d/demoreel100.hpp"
DemoReel100Ref demoReel = DemoReel100Ref::New(NUM_LEDS);

void initLED() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE, LED_PIN_2, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(MAXBRIGHTNESS);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;

 // gPal = CRGBPalette16(CRGB::Black, CRGB::Pink, CRGB::Pink, CRGB::White);

  //FillLEDsFromPaletteColors();

  chooseRandomPalette();

  demoInit();

 // gPal = HeatColors_p;
}

//currentBlending = LINEARBLEND;
static uint8_t startIndex = 0;

int motionSpeed = 3;
CRGB leds_L[NUM_LEDS];
CRGB leds_R[NUM_LEDS];

int maxPosLED = 0;

int maxRangePC = 0;

int firstPixel;  // = map(maxRangePC, 0, 100, 0, 58);
int lastPixel;   //= map(maxRangePC, 0, NUM_LEDS, 0, 59);

unsigned long prevMill_changePalete = 0;
int intervalPaltteChangeMin = 2;



void runLED() {

  random16_add_entropy(random());

  if (millis() - prevMill_changePalete >= intervalPaltteChangeMin * 60 * 1000) {
    prevMill_changePalete = millis();

    chooseRandomPalette();
  }


  if (millis() - previousMillis_runLEDs >= intervalFPSled) {
    previousMillis_runLEDs = millis();

    if(mode == 0){
        Fire2012WithPalette();

     gradualDimRange(firstPixel, lastPixel, 25);
    } else if (mode == 1){
         demoReel->draw(Fx::DrawContext(millis(), leds));
    }
   

    FastLED.show();
  }
}



void demoInit(){
    demoReel->lazyInit();
}

void gradualDimRange(int startIdx, int endIdx, uint8_t finalDimFactor) {
  int rangeLength = endIdx - startIdx + 1;
  for (int i = 0; i < rangeLength; i++) {
    uint8_t dimFactor = map(i, 0, rangeLength - 1, 255, finalDimFactor);
    leds[startIdx + i].nscale8(dimFactor);
  }
}

void dimLastPixels(int numPixels, uint8_t dimFactor) {
  for (int i = NUM_LEDS - numPixels; i < NUM_LEDS; i++) {
    // leds[i].nscale8(dimFactor);
    leds_R[i].nscale8(dimFactor);
    leds_L[i].nscale8(dimFactor);
  }
}

void dimRange(int startIdx, int endIdx, uint8_t dimFactor) {
  for (int i = startIdx; i <= endIdx; i++) {
    leds[i].nscale8(dimFactor);
  }
}


void Fire2012WithPalette_L() {
  // Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < (NUM_LEDS - maxPosLED); i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < (NUM_LEDS - maxPosLED); j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    uint8_t colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(gPal, colorindex);
    int pixelnumber;
    //gReverseDirection = false;
    //if( gReverseDirection ) {
    //  pixelnumber = (NUM_LEDS-1) - j;
    //} else {
    pixelnumber = j;
    // }
    leds_L[pixelnumber] = color;
    //leds[pixelnumber] = color;
  }
}

void Fire2012WithPalette_R() {
  // Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < (NUM_LEDS - maxPosLED); i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < (NUM_LEDS - maxPosLED); j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    uint8_t colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(gPal, colorindex);
    int pixelnumber;
    //gReverseDirection = true;
    //if( gReverseDirection ) {
    pixelnumber = (NUM_LEDS - 1) - j;
    // } else {
    //   pixelnumber = j;
    // }
    leds_R[pixelnumber] = color;
  }
}


void setMaxBrightness(int bri) {
  FastLED.setBrightness(bri);
}

void chooseRandomPalette() {
  randomSeed(analogRead(A5));
  int randomNum = random(0, 7);
  Serial.print(" Ranbom: ");
  Serial.println(randomNum);

  if (randomNum == 0) {
    currentPalette = RainbowStripeColors_p;
  } else if (randomNum == 1) {
    currentPalette = PinkPalette;
  } else if (randomNum == 2) {
    currentPalette = PartyColors_p;
  } else if (randomNum == 3) {
    currentPalette = CloudColors_p;
  } else if (randomNum == 4) {
    currentPalette = HeatColors_p;
  } else if (randomNum == 5) {
    currentPalette = LavaColors_p;
  } else if (randomNum == 6) {
    currentPalette = OceanColors_p;
  } else {
    currentPalette = PartyColors_p;
  }

  gPal = currentPalette;
  currentBlending = LINEARBLEND;
}

unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 100;  // interval at which to blink (milliseconds)


// void readPot() {
//   unsigned long currentMillis = millis();

//   if (currentMillis - previousMillis >= interval) {
//     // save the last time you blinked the LED
//     previousMillis = currentMillis;

//     int sensorValue = analogRead(sensorPin);
//     int brightness = map(sensorValue, 0, 4095, 0, 100);
//     FastLED.setBrightness(brightness);
//   }
// }

void FillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically() {
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand) {
    lastSecond = secondHand;

    Serial.println(secondHand);
    if (secondHand == 0) {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 10) {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 15) {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 20) {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 25) {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 30) {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if (secondHand == 35) {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 40) {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 45) {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 50) {
      currentPalette = PinkPalette;
      currentBlending = NOBLEND;
    }
    if (secondHand == 55) {
      currentPalette = PinkPalette;
      currentBlending = LINEARBLEND;
    }
  }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette() {
  for (int i = 0; i < 16; ++i) {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette() {
  // 'black out' all 16 palette entries...
  fill_solid(currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette() {
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green = CHSV(HUE_GREEN, 255, 255);
  CRGB black = CRGB::Black;

  currentPalette = CRGBPalette16(
    green, green, black, black,
    purple, purple, black, black,
    green, green, black, black,
    purple, purple, black, black);
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 PinkPalette PROGMEM = {
  CRGB::DeepPink,
  CRGB::Gray,  // 'white' is too bright compared to red and blue
  CRGB::DeepPink,
  CRGB::Black,

  CRGB::DeepPink,
  CRGB::Gray,
  CRGB::DeepPink,
  CRGB::Black,

  CRGB::DeepPink,
  CRGB::DeepPink,
  CRGB::DeepPink,
  CRGB::DeepPink,
  CRGB::DeepPink,
  CRGB::DeepPink,
  CRGB::Gray,
  CRGB::Gray
};

const TProgmemPalette16 YellowPalette PROGMEM = {
  CRGB::Yellow,
  CRGB::Gray,  // 'white' is too bright compared to red and blue
  CRGB::Yellow,
  CRGB::Black,

  CRGB::Yellow,
  CRGB::Gray,
  CRGB::Yellow,
  CRGB::Black,

  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Yellow,
  CRGB::Gray,
  CRGB::Gray
};


void Fire2012WithPalette() {
  // Array of temperature readings at each simulation cell
  static uint8_t heat[NUM_LEDS];

  // Step 1.  Cool down every cell a little
  for (int i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8(heat[i], random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for (int k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if (random8() < SPARKING) {
    int y = random8(7);
    heat[y] = qadd8(heat[y], random8(160, 255));
  }

  // Step 4.  Map from heat cells to LED colors
  for (int j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    uint8_t colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(gPal, colorindex);
    int pixelnumber;
    if (gReverseDirection) {
      pixelnumber = (NUM_LEDS - 1) - j;
    } else {
      pixelnumber = j;
    }
    leds[pixelnumber] = color;
  }
}



// Additional notes on FastLED compact palettes:
//
// Normally, in computer graphics, the palette (or "color lookup table")
// has 256 entries, each containing a specific 24-bit RGB color.  You can then
// index into the color palette using a simple 8-bit (one byte) value.
// A 256-entry color palette takes up 768 bytes of RAM, which on Arduino
// is quite possibly "too many" bytes.
//
// FastLED does offer traditional 256-element palettes, for setups that
// can afford the 768-byte cost in RAM.
//
// However, FastLED also offers a compact alternative.  FastLED offers
// palettes that store 16 distinct entries, but can be accessed AS IF
// they actually have 256 entries; this is accomplished by interpolating
// between the 16 explicit entries to create fifteen intermediate palette
// entries between each pair.
//
// So for example, if you set the first two explicit entries of a compact
// palette to Green (0,255,0) and Blue (0,0,255), and then retrieved
// the first sixteen entries from the virtual palette (of 256), you'd get
// Green, followed by a smooth gradient from green-to-blue, and then Blue.