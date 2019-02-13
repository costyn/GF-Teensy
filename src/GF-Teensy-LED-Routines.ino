#include <FastLED.h>
#include <math.h>


#define P_MAX_POS_ACCEL 3000

// How wide the bands of color are.  1 = more like a gradient, 10 = more like stripes
#if defined(JELLY) || defined(XMAS)
#define STEPS       5
#else
#define STEPS       1
#endif

void FillLEDsFromPaletteColors(uint8_t paletteIndex ) {
  static uint8_t startIndex = 15;  // initialize at start
#ifdef JELLY
  static int flowDir = -1 ;
#else
  static int flowDir = 1 ;
#endif

  const CRGBPalette16 palettes[] = { RainbowColors_p,
#ifdef RT_P_RB_STRIPE
                                     RainbowStripeColors_p,
#endif
#ifdef RT_P_OCEAN
                                     OceanColors_p,
#endif
#ifdef RT_P_HEAT
                                     HeatColors_p,
#endif
#ifdef RT_P_LAVA
                                     LavaColors_p,
#endif
#ifdef RT_P_PARTY
                                     PartyColors_p,
#endif
#ifdef RT_P_CLOUD
                                     CloudColors_p,
#endif
#ifdef RT_P_FOREST
                                     ForestColors_p
#endif
                                   } ;
  // Check our orientation and adjust flow direction accordingly
#ifdef USING_MPU
  if ( isMpuUp() ) {
    flowDir = -1 ;
  } else if ( isMpuDown() ) {
    flowDir = 1 ;
  }
#endif

  startIndex += flowDir ;

  uint8_t colorIndex = startIndex ;

  for ( uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( palettes[paletteIndex], colorIndex, 255, LINEARBLEND );
    colorIndex += STEPS;
  }

  #if ! defined(BALLOON) && ! defined(JELLY) && ! defined(GLOWSTAFF)
  //add extra glitter during "fast"
  if ( taskLedModeSelect.getInterval() < 5000 ) {
    addGlitter(250);
  } else {
    addGlitter(25);
  }
  #endif

#ifdef USING_MPU
  FastLED.setBrightness( map( constrain(aaRealZ, 0, P_MAX_POS_ACCEL), 0, P_MAX_POS_ACCEL, currentBrightness, 10 )) ;
#else
  FastLED.setBrightness( currentBrightness );
#endif
  FastLED.show();

//#if defined(GLOWSTAFF) || defined(BALLOON)
#if defined(RING) || defined( HOOP )
  taskLedModeSelect.setInterval( beatsin16( tapTempo.getBPM(), 1500, 50000) ) ;
#else
  taskLedModeSelect.setInterval( tapTempo.getBeatLength()*50 ) ;
#endif
}


#ifdef RT_FADE_GLITTER
void fadeGlitter() {
  addGlitter(70);
  uint16_t extraBright = round(currentBrightness * BRIGHTFACTOR) + currentBrightness ; // Add 50% brightness
  #ifdef ESP8266
    FastLED.setBrightness( _max(extraBright,255) ) ; // but restrict it to 255
  #else
    FastLED.setBrightness( max(extraBright,255) ) ; // but restrict it to 255
  #endif
  FastLED.show();
  fadeToBlackBy(leds, NUM_LEDS, 50);
}
#endif

#ifdef RT_DISCO_GLITTER
void discoGlitter() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
#ifdef USING_MPU
  addGlitter(map( constrain( activityLevel(), 0, 3000), 0, 3000, 100, 255 ));
#else
  addGlitter( 255 ) ;
#endif
  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
}
#endif

#ifdef RT_STROBE1
#define FLASHLENGTH 20
void strobe1() {
  if ( tapTempo.beatProgress() > 0.95 ) {
#ifdef USING_MPU
    fill_solid(leds, NUM_LEDS, CHSV( map( yprX, 0, 360, 0, 255 ), 255, 255)); // yaw for color
#else
    fill_solid(leds, NUM_LEDS, CHSV( 0, 255, 255)); // yaw for color
#endif
  } else if ( tapTempo.beatProgress() > 0.80 and tapTempo.beatProgress() < 0.85 ) {
    fill_solid(leds, NUM_LEDS, CRGB::White );
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black); // black
  }
  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
}
#endif

#ifdef RT_STROBE2
#define S_SENSITIVITY 3500  // lower for less movement to trigger accelerometer routines

void strobe2() {
  if ( activityLevel() > S_SENSITIVITY ) {
    fill_solid(leds, NUM_LEDS, CHSV( map( yprX, 0, 360, 0, 255 ), 255, currentBrightness)); // yaw for color
  } else {
    fadeall(120);
  }
  FastLED.show();
}
#endif


#ifdef RT_FIRE2012
// Fire2012 by Mark Kriegsman, July 2012

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
#define COOLING  75
// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 70

void Fire2012()
{
  static bool gReverseDirection = true;
  // Array of temperature readings at each simulation cell
    static byte heat[NUM_LEDS];

    // Step 1.  Cool down every cell a little
      for( int i = 0; i < NUM_LEDS; i++) {
        heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
      }

      // Step 2.  Heat from each cell drifts 'up' and diffuses a little
      for( int k= NUM_LEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
      }

      // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
      if( random8() < SPARKING ) {
        int y = random8(7);
        heat[y] = qadd8( heat[y], random8(160,255) );
      }

      // Step 4.  Map from heat cells to LED colors
      for( int j = 0; j < NUM_LEDS; j++) {
        CRGB color = HeatColor( heat[j]);
        int pixelnumber;
        if( gReverseDirection ) {
          pixelnumber = (NUM_LEDS-1) - j;
        } else {
          pixelnumber = j;
        }
        leds[pixelnumber] = color;
      }
  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();

  #ifdef USING_MPU
    if ( isMpuUp() ) {
      gReverseDirection = true;
    } else if ( isMpuDown() ) {
      gReverseDirection = false ;
    }

// map yprX to SPARKING
  #endif


} // end Fire2012
#endif


#ifdef RT_RACERS
void racingLeds() {
  //  static long loopCounter = 0 ;
  static uint8_t racer[] = {0, 1, 2, 3}; // Starting positions
  static int racerDir[] = {1, 1, 1, 1}; // Current direction
  static int racerSpeed[] = { random8(1, 4), random8(1, 4) , random8(1, 4), random8(1, 4) }; // Starting speed
  const CRGB racerColor[] = { CRGB::Red, CRGB::Blue, CRGB::White, CRGB::Orange }; // Racer colors

#define NUMRACERS sizeof(racer) //array size

  fill_solid(leds, NUM_LEDS, CRGB::Black);    // Start with black slate

  for ( uint8_t i = 0; i < NUMRACERS ; i++ ) {
    leds[racer[i]] = racerColor[i]; // Assign color

    // If taskLedModeSelect.getRunCounter() is evenly divisible by 'speed' then check if we've reached the end (if so, reverse), and do a step
    if ( ( taskLedModeSelect.getRunCounter() % racerSpeed[i]) == 0 ) {
      if ( racer[i] + racerDir[i] >= NUM_LEDS) {
        racer[i] = 0 ;
      } else {
        racer[i] += racerDir[i] ;
      }
      /*
            if ( (racer[i] + racerDir[i] >= NUM_LEDS) or (racer[i] + racerDir[i] <= 0) ) {
              racerDir[i] *= -1 ;
            }
            racer[i] += racerDir[i] ;
      */
    }

    if ( (taskLedModeSelect.getRunCounter() % 40 ) == 0 ) {
      racerSpeed[i] = random8(2, 6) ;  // Randomly speed up or slow down
    }
  }

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
} // end racers()
#endif


#ifdef RT_WAVE
#define WAVE_MAX_NEG_ACCEL -5000
#define WAVE_MAX_POS_ACCEL 5000
#define MIN_BRIGHT 20

void waveYourArms() {
  // Use yaw for color; use accelZ for brightness
  fill_solid(leds, NUM_LEDS, CHSV( map( yprX, 0, 360, 0, 255 ) , 255, map( constrain(aaRealZ, WAVE_MAX_NEG_ACCEL, WAVE_MAX_POS_ACCEL), WAVE_MAX_NEG_ACCEL, WAVE_MAX_POS_ACCEL, MIN_BRIGHT, 255 )) );

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
}
#endif


#ifdef RT_SHAKE_IT
#define SENSITIVITY 3000  // lower for less movement to trigger

void shakeIt() {
  uint8_t startLed = 0 ;

  if ( isMpuUp() ) {  // Start near controller if down
    startLed = 0 ;
  } else if ( isMpuDown() ) {
    startLed = NUM_LEDS - 1 ;
  }

  if ( activityLevel() > SENSITIVITY ) {
    leds[startLed] = CHSV( map( yprX, 0, 360, 0, 255 ), 255, 255); // yaw for color
  } else {
    leds[startLed] = CHSV(0, 0, 0); // black
  }

  //  for (int8_t i = NUM_LEDS - 2; i >= 0 ; i--) {
  //    leds[i + 1] = leds[i];
  //  }

  if ( isMpuUp() ) {
    for (int8_t i = NUM_LEDS - 2; i >= 0 ; i--) {
      leds[i + 1] = leds[i];
    }
  } else if ( isMpuDown() ) {
    for (int8_t i = 0 ; i <= NUM_LEDS - 2 ; i++) {
      leds[i] = leds[i + 1];
    }
  }


  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
}
#endif


#ifdef WHITESTRIPE
#define STRIPE_LENGTH 5

// This routines goes "over" other patterns, remembering/copying the
// pattern it is writing over and writing it back behind it.

void whiteStripe() {
  static CRGB patternCopy[STRIPE_LENGTH] ;
  static int startLed = 0 ;

  if ( taskWhiteStripe.getInterval() != WHITESTRIPE_SPEED ) {
    taskWhiteStripe.setInterval( WHITESTRIPE_SPEED ) ;
  }

  if ( startLed == 0 ) {
    for (uint8_t i = 0; i < STRIPE_LENGTH ; i++ ) {
      patternCopy[i] = leds[i];
    }
  }

  // 36 40   44 48 52 56   60

  leds[startLed] = patternCopy[0] ;
  for (uint8_t i = 0; i < STRIPE_LENGTH - 2; i++ ) {
    patternCopy[i] = patternCopy[i + 1] ;
  }
  patternCopy[STRIPE_LENGTH - 1] = leds[startLed + STRIPE_LENGTH] ;

  fill_gradient(leds, startLed + 1, CHSV(0, 0, 255), startLed + STRIPE_LENGTH, CHSV(0, 0, 255), SHORTEST_HUES);

  startLed++ ;

  if ( startLed + STRIPE_LENGTH == NUM_LEDS - 1) { // LED nr 90 is index 89
    for (uint8_t i = startLed; i < startLed + STRIPE_LENGTH; i++ ) {
      leds[i] = patternCopy[i];
    }

    startLed = 0 ;
    taskWhiteStripe.setInterval(random16(4000, 10000)) ;
  }

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
}
#endif


#ifdef RT_GLED_ORIGINAL
void gLedOrig() {
  leds[lowestPoint()] = ColorFromPalette( PartyColors_p, taskLedModeSelect.getRunCounter(), currentBrightness, NOBLEND );
  FastLED.show();
  fadeToBlackBy(leds, NUM_LEDS, 200);
}
#endif


#ifdef RT_GLED
// Gravity LED: lights up a small gradient at the lowest point on the ring
#define GLED_WIDTH 3
void gLed() {
  uint8_t ledPos = lowestPoint() ;
  static uint8_t hue = 0 ;
  fillGradientRing( ledPos, CHSV(hue, 255, 0) , ledPos + GLED_WIDTH , CHSV(hue, 255, 255) ) ;
  fillGradientRing( ledPos + GLED_WIDTH + 1, CHSV(hue, 255, 255), ledPos + GLED_WIDTH + GLED_WIDTH, CHSV(hue, 255, 0) ) ;
  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
  hue++ ;
}
#endif



#if defined(RT_TWIRL1) || defined(RT_TWIRL2) || defined(RT_TWIRL4) || defined(RT_TWIRL6) || defined(RT_TWIRL2_O) || defined(RT_TWIRL4_O) || defined(RT_TWIRL6_O)
// Counter rotating twirlers with blending
// 1 twirler - 1 white = 120/1
// 2 twirler - 1 white = 120/1
// 4 twirler - 2 white = 120/2
// 6 twirler - 3 white = 120/3

void twirlers(uint8_t numTwirlers, bool opposing ) {
  uint8_t pos = 0 ;
  uint8_t speedCorrection = 0 ;

  if ( numTwirlers == 1 ) {
    speedCorrection = 1 ;
  } else {
    speedCorrection = numTwirlers / 2 ;
  }
  uint8_t clockwiseFirst = lerp8by8( 0, NUM_LEDS, beat8( tapTempo.getBPM() / speedCorrection )) ;
  const CRGB clockwiseColor = CRGB::White ;
  const CRGB antiClockwiseColor = CRGB::Red ;

  if ( opposing ) {
    fadeall(map( numTwirlers, 1, 6, 250, 230 ));
  } else {
    fadeall(map( numTwirlers, 1, 6, 250, 230 ));
  }

  for (uint8_t i = 0 ; i < numTwirlers ; i++) {
    if ( (i % 2) == 0 ) {
      pos = (clockwiseFirst + round( NUM_LEDS / numTwirlers ) * i) % NUM_LEDS ;
      if ( leds[pos] ) { // FALSE if currently BLACK - don't blend with black
        leds[pos] = blend( leds[pos], clockwiseColor, 128 ) ;
      } else {
        leds[pos] = clockwiseColor ;
      }

    } else {

      if ( opposing ) {
        uint8_t antiClockwiseFirst = NUM_LEDS - (lerp8by8( 0, NUM_LEDS, beat8( tapTempo.getBPM() / speedCorrection ))) % NUM_LEDS ;
        pos = (antiClockwiseFirst + round( NUM_LEDS / numTwirlers ) * i) % NUM_LEDS ;
      } else {
        pos = (clockwiseFirst + round( NUM_LEDS / numTwirlers ) * i) % NUM_LEDS ;
      }
      if ( leds[pos] ) { // FALSE if currently BLACK - don't blend with black
        leds[pos] = blend( leds[pos], antiClockwiseColor, 128 ) ;
      } else {
        leds[pos] = antiClockwiseColor ;
      }
    }

  }
  uint16_t extraBright = round(currentBrightness * BRIGHTFACTOR) + currentBrightness ; // Add 50% brightness
  #ifdef ESP8266
    FastLED.setBrightness( _max(extraBright,255) ) ; // but restrict it to 255
  #else
    FastLED.setBrightness( max(extraBright,255) ) ; // but restrict it to 255
  #endif
  FastLED.show();
//  taskLedModeSelect.setInterval( 1 * TASK_RES_MULTIPLIER ) ;
}
#endif


#ifdef RT_HEARTBEAT
void heartbeat() {
  const uint8_t hbTable[] = {
    25,
    61,
    105,
    153,
    197,
    233,
    253,
    255,
    252,
    243,
    230,
    213,
    194,
    149,
    101,
    105,
    153,
    197,
    216,
    233,
    244,
    253,
    255,
    255,
    252,
    249,
    243,
    237,
    230,
    223,
    213,
    206,
    194,
    184,
    174,
    162,
    149,
    138,
    126,
    112,
    101,
    91,
    78,
    69,
    62,
    58,
    51,
    47,
    43,
    39,
    37,
    35,
    29,
    25,
    22,
    20,
    19,
    15,
    12,
    9,
    8,
    6,
    5,
    3
  };

#define NUM_STEPS (sizeof(hbTable)/sizeof(uint8_t)) //array size
  //#define NUM_STEPS 64
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  // beat8 generates index 0-255 (fract8) as per getBPM(). lerp8by8 interpolates that to array index:
  uint8_t hbIndex = lerp8by8( 0, NUM_STEPS, beat8( tapTempo.getBPM() / 2 )) ;
  uint8_t brightness = lerp8by8( 0, 255, hbTable[hbIndex] ) ;
  //  DEBUG_PRINT(NUM_STEPS) ;
  //  DEBUG_PRINT(F("\t")) ;
  //  DEBUG_PRINT(hbIndex) ;
  //  DEBUG_PRINT(F("\t")) ;
  //  DEBUG_PRINT(brightness) ;
  //  DEBUG_PRINTLN() ;

  FastLED.setBrightness( brightness );
  FastLED.show();
}
#endif


#if defined(RT_FASTLOOP) || defined(RT_FASTLOOP2)

#define FL_LENGHT 20   // how many LEDs should be in the "stripe"
#define FL_MIDPOINT FL_LENGHT / 2
#define MAX_LOOP_SPEED 5

void fastLoop(bool reverse) {
  static int16_t startP = 0 ;
  static uint8_t hue = 0 ;

  if ( ! reverse ) {
    startP = lerp8by8( 0, NUM_LEDS, beat8( tapTempo.getBPM() )) ;  // start position
  } else {
    startP += map( sin8( beat8( tapTempo.getBPM() / 4 )), 0, 255, -MAX_LOOP_SPEED, MAX_LOOP_SPEED + 1 ) ; // it was hard to write, it should be hard to undestand :grimacing:
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fillGradientRing(startP, CHSV(hue, 255, 0), startP + FL_MIDPOINT, CHSV(hue, 255, 255));
  fillGradientRing(startP + FL_MIDPOINT + 1, CHSV(hue, 255, 255), startP + FL_LENGHT, CHSV(hue, 255, 0));

  uint16_t extraBright = round(currentBrightness * BRIGHTFACTOR) + currentBrightness ; // Add 50% brightness
  #ifdef ESP8266
    FastLED.setBrightness( _max(extraBright,255) ) ; // but restrict it to 255
  #else
    FastLED.setBrightness( max(extraBright,255) ) ; // but restrict it to 255
  #endif
  FastLED.show();
  hue++  ;

}
#endif


#if defined(RT_NOISE_LAVA) || defined(RT_NOISE_PARTY) || defined(RT_NOISE_OCEAN)
// FastLED library NoisePlusPalette routine rewritten for 1 dimensional LED strip
// - speed determines how fast time moves forward.  Try  1 for a very slow moving effect,
// or 60 for something that ends up looking like water.

// - Scale determines how far apart the pixels in our noise array are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise will be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.

// if current palette is a 'loop', add a slowly-changing base value

void fillnoise8(uint8_t currentPalette, uint8_t speed, uint8_t scale, boolean colorLoop ) {
  static uint8_t noise[NUM_LEDS];

  const CRGBPalette16 palettes[] = { LavaColors_p, PartyColors_p, OceanColors_p } ;

  static uint16_t x = random16();
  static uint16_t y = random16();
  static uint16_t z = random16();

  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;

  if ( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    int ioffset = scale * i;

    uint8_t data = inoise8(x + ioffset, y, z);

    // The range of the inoise8 function is roughly 16-238.
    // These two operations expand those values out to roughly 0..255
    // You can comment them out if you want the raw noise data.
    data = qsub8(data, 16);
    data = qadd8(data, scale8(data, 39));

    if ( dataSmoothing ) {
      uint8_t olddata = noise[i];
      uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
      data = newdata;
    }

    noise[i] = data;
  }

  z += speed;

  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;

  static uint8_t ihue = 0;

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    // We use the value at the i coordinate in the noise
    // array for our brightness, and a 'random' value from NUM_LEDS - 1
    // for our pixel's index into the color palette.

    uint8_t index = noise[i];
    uint8_t bri =   noise[NUM_LEDS - i];
    // uint8_t bri =  sin(noise[NUM_LEDS - i]);  // more light/dark variation

    // if this palette is a 'loop', add a slowly-changing base value
    if ( colorLoop) {
      index += ihue;
    }

    // brighten up, as the color palette itself often contains the
    // light/dark dynamic range desired
    if ( bri > 127 ) {
      bri = 255;
    } else {
      bri = dim8_raw( bri * 2);
    }

    CRGB color = ColorFromPalette( palettes[currentPalette], index, bri);
    leds[i] = color;
  }
  ihue += 1;

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
}
#endif


#ifdef RT_PENDULUM
void pendulum() {
#ifdef USING_MPU
  uint8_t hue = map( yprX, 0, 360, 0, 255 ) ; // yaw for color
#else
  uint8_t hue = 0 ; // yaw for color
#endif
  uint8_t sPos1 = beatsin8( tapTempo.getBPM(), 0, NUM_LEDS / 2 ) ;
  uint8_t sPos2 = beatsin8( tapTempo.getBPM(), NUM_LEDS / 2, NUM_LEDS ) ;
  fillGradientRing(sPos1, CHSV(hue, 255, 0), sPos1 + 10, CHSV(hue, 255, 255));
  fillGradientRing(sPos1 + 11, CHSV(hue, 255, 255), sPos1 + 20, CHSV(hue, 255, 0));
  fillGradientRing(sPos2, CHSV(hue + 128, 255, 0), sPos2 + 10, CHSV(hue + 128, 255, 255));
  fillGradientRing(sPos2 + 11, CHSV(hue + 128, 255, 255), sPos2 + 20, CHSV(hue + 128, 255, 0));
  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
} // end pendulum()
#endif



#ifdef RT_BOUNCEBLEND
void bounceBlend() {
  uint8_t speed = beatsin8( tapTempo.getBPM(), 0, 255);
  static uint8_t startLed = 1 ;
  CHSV endclr = blend(CHSV(0, 255, 255), CHSV(160, 255, 0) , speed);
  CHSV midclr = blend(CHSV(160, 255, 0) , CHSV(0, 255, 255) , speed);
  fillGradientRing(startLed, endclr, startLed + NUM_LEDS / 2, midclr);
  fillGradientRing(startLed + NUM_LEDS / 2 + 1, midclr, startLed + NUM_LEDS, endclr);

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();

  if ( (taskLedModeSelect.getRunCounter() % 10 ) == 0 ) {
    startLed++ ;
    if ( startLed + 1 == NUM_LEDS ) startLed = 0  ;
  }
} // end bounceBlend()
#endif


/* juggle_pal
   Originally by: Mark Kriegsman
   Modified by: Andrew Tuline
   Modified further by: Costyn van Dongen
   Date: May, 2017
*/

#ifdef RT_JUGGLE_PAL
void jugglePal() {                                             // A time (rather than loop) based demo sequencer. This gives us full control over the length of each sequence.

  static uint8_t    numdots =   4;                                     // Number of dots in use.
  static uint8_t   thisfade =   2;                                     // How long should the trails be. Very low value = longer trails.
  static uint8_t   thisdiff =  16;                                     // Incremental change in hue between each dot.
  static uint8_t    thishue =   0;                                     // Starting hue.
  static uint8_t     curhue =   0;                                     // The current hue
  static uint8_t   thisbeat =   35;                                     // Higher = faster movement.
  static float   fadeFactor = 1.00;                                     // 120 is reference BPM. Fade values are calculated for that.

  uint8_t secondHand = (millis() / 1000) % 60;                // Change '60' to a different value to change duration of the loop (also change timings below)
  static uint8_t lastSecond = 99;                             // This is our 'debounce' variable.



  if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch (secondHand) {
      case  1: numdots = 1; thisbeat = tapTempo.getBPM() / 2; thisdiff = 8;  thisfade = (int)8*fadeFactor;  thishue = 0;   break;
      case  6: numdots = 2; thisbeat = tapTempo.getBPM() / 2; thisdiff = 4;  thisfade = (int)12*fadeFactor; thishue = 0;   break;
      case 25: numdots = 4; thisbeat = tapTempo.getBPM() / 2; thisdiff = 24; thisfade = (int)50*fadeFactor; thishue = 128; break;
      case 40: numdots = 2; thisbeat = tapTempo.getBPM() / 2; thisdiff = 16; thisfade = (int)50*fadeFactor; thishue = 0; break;
      case 52: numdots = 4; thisbeat = tapTempo.getBPM() / 2; thisdiff = 24; thisfade = (int)80*fadeFactor; thishue = 160; break;
    }
    fadeFactor = tapTempo.getBPM() / 120 ;
  }

  curhue = thishue;                                           // Reset the hue values.
  fadeToBlackBy(leds, NUM_LEDS, thisfade);

  for ( uint8_t i = 0; i < numdots; i++) {
    uint8_t whichLED = beatsin16(thisbeat + i + numdots, 0, NUM_LEDS - 1);
    // if( numdots == 1 ) {
    //   DEBUG_PRINT(whichLED);
    //   DEBUG_PRINT(" ");
    //   DEBUG_PRINTLN( micros() );
    // }
    leds[whichLED] += ColorFromPalette(RainbowColors_p, curhue, 255, LINEARBLEND); // Munge the values and pick a colour from the palette
    curhue += thisdiff;
  }

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();

} // end jugglePal()
#endif


#ifdef RT_QUAD_STROBE

// TODO: make strobes shorter
void quadStrobe() {
  static uint8_t shift = 0 ;
  uint8_t triwave = triwave8( taskLedModeSelect.getRunCounter() * 6 ) ;
  uint8_t striplength = lerp8by8( 1, 16, triwave ) ;
  uint8_t startP = mod( taskLedModeSelect.getRunCounter() * 15 + shift, NUM_LEDS ) ;

  fill_solid( leds, NUM_LEDS, CRGB::Black ) ;
  fillSolidRing( startP, startP + striplength, CHSV(0, 0, 255) ) ; // white

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();

  if ( striplength == 1 ) shift++ ; // shift the sequence on clockwise
} // end quadStrobe()
#endif


#ifdef RT_PULSE_3
#define PULSE_WIDTH 10
void pulse3() {
  uint8_t width = beatsin8( constrain( tapTempo.getBPM() * 2, 0, 255), 0, PULSE_WIDTH ) ; // can't use BPM > 255
  uint8_t hue = beatsin8( 1, 0, 255) ;
  static uint8_t middle = 0 ;

  if ( width == 1 ) {
    middle = taskLedModeSelect.getRunCounter() % 60 + taskLedModeSelect.getRunCounter() % 2;
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fillGradientRing(middle - width, CHSV(hue, 255, 0), middle, CHSV(hue, 255, 255));
  fillGradientRing(middle, CHSV(hue, 255, 255), middle + width, CHSV(hue, 255, 0));

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show() ;
}
#endif

#if defined(RT_PULSE_5_1) || defined(RT_PULSE_5_2) || defined(RT_PULSE_5_3)
void pulse5( uint8_t numPulses, boolean leadingDot) {
  uint8_t spacing = NUM_LEDS / numPulses ;
  uint8_t pulseWidth = (spacing / 2) - 1 ; // leave 1 led empty at max
  uint8_t middle = beatsin8( 10, 0, NUM_LEDS / 2) ;
  uint8_t width = beatsin8( tapTempo.getBPM(), 0, pulseWidth) ;
#ifdef USING_MPU
  uint8_t hue = map( yprX, 0, 360, 0, 255 ) ;
#else
  uint8_t hue = 180 ;
#endif

  fill_solid(leds, NUM_LEDS, CRGB::Black);

  for ( uint8_t i = 0 ; i < numPulses; i++ ) {
    uint8_t offset = spacing * i ;
    fillGradientRing(middle - width + offset, CHSV(hue, 255, 0), middle + offset, CHSV(hue, 255, 255));
    fillGradientRing(middle + offset, CHSV(hue, 255, 255), middle + width + offset, CHSV(hue, 255, 0));

    if ( leadingDot ) {  // abusing fill gradient since it deals with "ring math"
      fillGradientRing(middle - width + offset, CHSV(0, 255, 255), middle - width + offset, CHSV(0, 255, 255));
      fillGradientRing(middle + width + offset, CHSV(0, 255, 255), middle + width + offset, CHSV(0, 255, 255));
    }
  }

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show() ;
}
#endif



#ifdef RT_THREE_SIN_PAL

/* three_sin_pal_demo
   By: Andrew Tuline
   Date: March, 2015
   3 sine waves, one for each colour. I didn't take this far, but you could change the beat frequency and so on. . .
*/
#define MAXCHANGES 24
// Frequency, thus the distance between waves:
#define MUL1 7
#define MUL2 6
#define MUL3 5
void threeSinPal() {
  static int wave1 = 0;                                                // Current phase is calculated.
  static int wave2 = 0;
  static int wave3 = 0;

  static CRGBPalette16 currentPalette(CRGB::Black);
  static CRGBPalette16 targetPalette(PartyColors_p);

  if ( taskLedModeSelect.getRunCounter() % 2 == 0 ) {
    nblendPaletteTowardPalette( currentPalette, targetPalette, MAXCHANGES);

    wave1 += beatsin8(10, -4, 4);
    wave2 += beatsin8(15, -2, 2);
    wave3 += beatsin8(12, -3, 3);

    for (int k = 0; k < NUM_LEDS; k++) {
      uint8_t tmp = sin8(MUL1 * k + wave1) + sin8(MUL2 * k + wave2) + sin8(MUL3 * k + wave3);
      leds[k] = ColorFromPalette(currentPalette, tmp, 255);
    }
  }

  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if ( lastSecond != secondHand) {
    lastSecond = secondHand;
    CRGB p = CHSV( HUE_PURPLE, 255, 255);
    CRGB g = CHSV( HUE_GREEN, 255, 255);
    CRGB u = CHSV( HUE_BLUE, 255, 255);
    CRGB b = CRGB::Black;
    CRGB w = CRGB::White;

    switch (secondHand) {
      case  0: targetPalette = RainbowColors_p; break;
      case  5: targetPalette = CRGBPalette16( u, u, b, b, p, p, b, b, u, u, b, b, p, p, b, b); break;
      case 10: targetPalette = OceanColors_p; break;
      case 15: targetPalette = CloudColors_p; break;
      case 20: targetPalette = LavaColors_p; break;
      case 25: targetPalette = ForestColors_p; break;
      case 30: targetPalette = PartyColors_p; break;
      case 35: targetPalette = CRGBPalette16( b, b, b, w, b, b, b, w, b, b, b, w, b, b, b, w); break;
      case 40: targetPalette = CRGBPalette16( u, u, u, w, u, u, u, w, u, u, u, w, u, u, u, w); break;
      case 45: targetPalette = CRGBPalette16( u, p, u, w, p, u, u, w, u, g, u, w, u, p, u, w); break;
      case 50: targetPalette = CloudColors_p; break;
      case 55: targetPalette = CRGBPalette16( u, u, u, w, u, u, p, p, u, p, p, p, u, p, p, w); break;
      case 60: break;
    }
  }

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();

} // threeSinPal()
#endif



#ifdef RT_COLOR_GLOW
void colorGlow() {
  static uint8_t paletteColorIndex = 0 ;
  static bool indexUpdated = false ;
  uint8_t brightness = beatsin8( tapTempo.getBPM(), 0, 255 ) ;

  // To ensure we update paletteColorIndex only once per brightness cycle, use the flag indexUpdated.
  if( brightness < 5 and not indexUpdated ) {
    paletteColorIndex += 32 ;
    indexUpdated = true ;
    Serial.println(paletteColorIndex);
  }

  if( brightness > 5 and indexUpdated ) {
    indexUpdated = false ;
//    Serial.println("going back up");
  }

  fill_solid(leds, NUM_LEDS, ColorFromPalette( RainbowColors_p, paletteColorIndex, brightness, LINEARBLEND ));
  // fill_solid(leds, NUM_LEDS, CRGB::Blue);

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show() ;
}
#endif

#ifdef RT_FAN_WIPE
void fanWipe() {
    uint8_t hue = beatsin8( 1, 0, 255) ;
//    uint8_t vertIndex = lerp8by8( 0, 6, triwave8( taskLedModeSelect.getRunCounter() % 128 ) * 2 ) ;
    uint8_t vertIndex = beatsin8( 45, 0, 5 ) ;

//    fill_solid(leds, NUM_LEDS, CRGB::Black);
//   DEBUG_PRINTLN(vertIndex) ;

    for(uint8_t blade = 0 ; blade < NUM_BLADES; blade++ ) {
      leds[vertIndex+(blade*6)] = CHSV(hue, 255, 255) ;
    }
    // if( vertIndex == 0 ) {
    // } else {
    //   leds[vertIndex-1] = CHSV(hue, 255, 255) ; // blade 0
    // }

    FastLED.setBrightness( currentBrightness ) ;
    FastLED.show() ;
    fadeToBlackBy(leds, NUM_LEDS, 25);
  }
#endif

#ifdef RT_DROPLETS
//#define STOPPING
void droplets() {
  //  static long loopCounter = 0 ;
  // static uint8_t droplet[] = { random8(0, NUM_LEDS - 1), random8(0, NUM_LEDS - 1), random8(0, NUM_LEDS - 1), random8(0, NUM_LEDS - 1) }; // Starting positions
  static uint8_t droplet[] = { 1, 1, 1, 1 }; // Starting positions
  static int dropletSpeed[] = { random8(1, 3), random8(1, 3) , random8(1, 3), random8(1, 3) }; // Starting speed
  //static int dropletSpeed[] = { 1, 1, 1, 1 }; // Starting speed
  const CRGB dropletColor[] = { CRGB::White, CRGB::White, CRGB::White, CRGB::White }; // droplet colors

  #define NUM_DROPLETS sizeof(droplet) //array size

  for ( uint8_t i = 0; i < NUM_DROPLETS ; i++ ) {
    leds[droplet[i]] = dropletColor[i]; // Assign droplet color
    leds[droplet[i]-1] = CRGB::Red ; // Assign tail

    #ifdef STOPPING
    if( random8(1,10) == 5 && dropletSpeed[i] < 2 ) {
      dropletSpeed[i] = random8(2, 3);
    } else if( random8(1,20) == 5 && dropletSpeed[i] >= 2 ) {
      dropletSpeed[i] = random8(0, 1);
    }

    if( random8(1,50) == 5 ) {
      droplet[i] = random8(1, NUM_LEDS - 10) ;
    }
    #endif

    // If taskLedModeSelect.getRunCounter() is evenly divisible by 'speed' then check if we've reached the end (if so, pick a new random starting point)
    if( ( taskLedModeSelect.getRunCounter() % dropletSpeed[i]) == 0 ) {
      if ( droplet[i] + 1 >= NUM_LEDS) {
        droplet[i] = random8(1, 30) ;
      } else {
        droplet[i] += 1 ;
      }

    }
  }

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
  fadeall(210);
} // end droplets()
#endif



#if defined(RT_POVPATTERNS) && defined(_TASK_MICRO_RES)

#define redVal   0
#define greenVal 1
#define blueVal  2

void povPatterns(unsigned long time, const char pattern[][NUM_LEDS][3], int pictureWidth)
{
  unsigned long currentTime = millis();

  while (millis() < currentTime + (time * 1000))
  {
    for(int slice = 0; slice < pictureWidth; slice ++)
    {
      for(uint8_t LED = NUM_LEDS-1; LED > -1; LED --) // LED number
      {
        leds[LED].setRGB(pattern[slice] [LED] [redVal],
                               pattern[slice] [LED] [greenVal],
                               pattern[slice] [LED] [blueVal]);
      }
      FastLED.show();
      taskLedModeSelect.delay(800); // How wide the image is
    }
    taskLedModeSelect.delay(1000); // Gap between images
  }
}

#endif


#ifdef RT_BOUNCYBALLS
// Code by Danny Wilson
// https://github.com/daterdots/LEDs/blob/master/BouncingBalls2014/BouncingBalls2014.ino

#define GRAVITY           -1              // Downward (negative) acceleration of gravity in m/s^2
#define h0                1                  // Starting height, in meters, of the ball (strip length)
#define NUM_BALLS         3                  // Number of bouncing balls you want (recommend < 7, but 20 is fun in its own way)

float h[NUM_BALLS] ;                         // An array of heights
float vImpact0 = sqrt( -2 * GRAVITY * h0 );  // Impact velocity of the ball when it hits the ground if "dropped" from the top of the strip
float vImpact[NUM_BALLS] ;                   // As time goes on the impact velocity will change, so make an array to store those values
float tCycle[NUM_BALLS] ;                    // The time since the last time the ball struck the ground
int   pos[NUM_BALLS] ;                       // The integer position of the dot on the strip (LED index)
long  tLast[NUM_BALLS] ;                     // The clock time of the last ground strike
float COR[NUM_BALLS] ;                       // Coefficient of Restitution (bounce damping)
boolean firstRun = true ;                    // ugly way to run init only once :(

void bouncyBalls() {
  if( firstRun ) {                             // Only run once
    for (int i = 0 ; i < NUM_BALLS ; i++) {    // Initialize variables
      tLast[i] = millis();
      h[i] = h0;
      pos[i] = 0;                              // Balls start on the ground
      vImpact[i] = vImpact0;                   // And "pop" up at vImpact0
      tCycle[i] = 0;
      COR[i] = 0.90 - float(i)/pow(NUM_BALLS,2);
    }
    firstRun = false ;
  }

  for (int i = 0 ; i < NUM_BALLS ; i++) {
    tCycle[i] =  millis() - tLast[i] ;     // Calculate the time since the last time the ball was on the ground

    // A little kinematics equation calculates positon as a function of time, acceleration (gravity) and intial velocity
    h[i] = 0.5 * GRAVITY * pow( tCycle[i]/1000 , 2.0 ) + vImpact[i] * tCycle[i]/1000;

    if ( h[i] < 0 ) {
      h[i] = 0;                            // If the ball crossed the threshold of the "ground," put it back on the ground
      vImpact[i] = COR[i] * vImpact[i] ;   // and recalculate its new upward velocity as it's old velocity * COR
      tLast[i] = millis();

      if ( vImpact[i] < 0.01 ) vImpact[i] = vImpact0;  // If the ball is barely moving, "pop" it back up at vImpact0
    }
    pos[i] = round( h[i] * (NUM_LEDS - 1) / h0);       // Map "h" to a "pos" integer index position on the LED strip
  }

  //Choose color of LEDs, then the "pos" LED on
  for (int i = 0 ; i < NUM_BALLS ; i++) {
    leds[pos[i]] = CHSV( uint8_t (i * 40) , 255, 255);
  }

  uint16_t extraBright = round(currentBrightness * BRIGHTFACTOR) + currentBrightness ; // Add 20% brightness
  FastLED.setBrightness( max(extraBright,255) ) ; // but restrict it to 255
  FastLED.show();
  //Then off for the next loop around
  for (int i = 0 ; i < NUM_BALLS ; i++) {
    leds[pos[i]] = CRGB::Black;
  }
}

#endif

#ifdef RT_DROPLETS2
  // pos = 0.5 * 9.8 * time

void droplets2() {
  static int pos = 0 ; // start from top
  static long dropStart = millis() ;
  static long elapsed ;

  elapsed = millis() - dropStart ;

  pos = round( 0.5 * 1 * elapsed * elapsed / 10000 ) ;
//  DEBUG_PRINTLN(pos);

  if( pos >= NUM_LEDS ) {
//    delay(200);
    FastLED.show() ;
    pos = 0 ; // reset to top
    dropStart = millis() ;
  }

  fadeall(120);

  leds[pos] = CRGB::White ;
  FastLED.show() ;
}

#endif



// Playing with easing http://crisbeto.github.io/angular-svg-round-progressbar/
#ifdef RT_CIRC_LOADER
void circularLoader() {
  AHFloat ah_float = 0.0 ;
  AHloat eased_float = 0.0 ;
  eased_float = CubicEaseIn( ah_float );

  uint8_t triwave = triwave8( taskLedModeSelect.getRunCounter() * 5 ) ;
  uint8_t striplength = lerp8by8( 2, 20, triwave ) ;
  static uint8_t startP = 50;

  fill_solid( leds, NUM_LEDS, CRGB::Black ) ;
  fillSolidRing( startP - striplength, startP, CHSV(0, 255, 255) ) ; // white

  FastLED.setBrightness( currentBrightness ) ;
  FastLED.show();
  startP = startP + lerp8by8( 2, 5, triwave ) ;
}
#endif



#if defined(RT_SINLOOP)

#define SL_LENGHT 20 // how many LEDs should be in the "stripe"
#define SL_MIDPOINT SL_LENGHT / 2
#define SL_NUMSTRIPES 3

void fastLoop3() {
  static int16_t startP = 0;
  static uint8_t hue = 0;

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (int i = 0; i < SL_NUMSTRIPES; i++) {
    startP = lerp8by8(0, NUM_LEDS, beat8(40 + (i * 3))); // 40, 43, 46
    fillGradientRing(startP, CHSV(hue + (i * 30), 255, 0), startP + SL_MIDPOINT,
                     CHSV(hue, 255, 255));
    fillGradientRing(startP + SL_MIDPOINT + 1, CHSV(hue, 255, 255),
                     startP + SL_LENGHT, CHSV(hue, 255, 0));
  }

  uint16_t extraBright = round(currentBrightness * BRIGHTFACTOR) +
                         currentBrightness; // Add 50% brightness
#ifdef ESP8266
  FastLED.setBrightness(_max(extraBright, 255)); // but restrict it to 255
#else
  FastLED.setBrightness(max(extraBright, 255)); // but restrict it to 255
#endif
  FastLED.show();
  hue++;
}
#endif



#ifdef RT_RIPPLE
// Adapted by Andrew Tuline from NeoPixel version
// https://pastebin.com/LfBsPLRn
// https://www.youtube.com/watch?v=IrMzopUe8F4

#define maxSteps 16
#define fadeRate 0.8

void ripple() {
  static uint32_t currentBg = random(256);
  static uint32_t nextBg = currentBg;
  static int color;
  static int center = 0;
  static int step = -1;

  if (currentBg == nextBg) {
    nextBg = random(256);
  } else if (nextBg > currentBg) {
    currentBg++;
  } else {
    currentBg--;
  }
  for (uint16_t l = 0; l < NUM_LEDS; l++) {
    leds[l] = CHSV(currentBg, 255, 50); // strip.setPixelColor(l, Wheel(currentBg, 0.1));
  }

  if (step == -1) {
    center = random(NUM_LEDS);
    color = random(256);
    step = 0;
  }

  if (step == 0) {
    leds[center] =
        CHSV(color, 255, 255); // strip.setPixelColor(center, Wheel(color, 1));
    step++;
  } else {
    if (step < maxSteps) {
      Serial.println(pow(fadeRate, step));

      leds[wrap(center + step)] =
          CHSV(color, 255,
               pow(fadeRate, step) *
                   255); //   strip.setPixelColor(wrap(center + step),
                         //   Wheel(color, pow(fadeRate, step)));
      leds[wrap(center - step)] =
          CHSV(color, 255,
               pow(fadeRate, step) *
                   255); //   strip.setPixelColor(wrap(center - step),
                         //   Wheel(color, pow(fadeRate, step)));
      if (step > 3) {
        leds[wrap(center + step - 3)] =
            CHSV(color, 255,
                 pow(fadeRate, step - 2) *
                     255); //   strip.setPixelColor(wrap(center + step - 3),
                           //   Wheel(color, pow(fadeRate, step - 2)));
        leds[wrap(center - step + 3)] =
            CHSV(color, 255,
                 pow(fadeRate, step - 2) *
                     255); //   strip.setPixelColor(wrap(center - step + 3),
                           //   Wheel(color, pow(fadeRate, step - 2)));
      }
      step++;
    } else {
      step = -1;
    }
  }

  FastLED.show();
}

int wrap(int step) {
  if(step < 0) return NUM_LEDS + step;
  if(step > NUM_LEDS - 1) return step - NUM_LEDS;
  return step;
}

void one_color_allHSV(int ahue, int abright) {                // SET ALL LEDS TO ONE COLOR (HSV)
  for (int i = 0 ; i < NUM_LEDS; i++ ) {
    leds[i] = CHSV(ahue, 255, abright);
  }
}
#endif


#ifdef RT_RANDOMWALK
// Source: https://www.arduino.cc/reference/en/language/variables/variable-scope--qualifiers/static/
// Thought it might be interesting

#define randomWalkLowRange  0
#define randomWalkHighRange NUM_LEDS
#define moveSize 5

void randomWalk(){
  static int  place;     // variable to store value in random walk - declared static so that it stores
                         // values in between function calls, but no other functions can change its value

  place = place + (random(-moveSize, moveSize + 1));

  if (place < randomWalkLowRange){                              // check lower and upper limits
    place = randomWalkLowRange + (randomWalkLowRange - place);  // reflect number back in positive direction
  }
  else if(place > randomWalkHighRange){
    place = randomWalkHighRange - (place - randomWalkHighRange);  // reflect number back in negative direction
  }

  leds[place] = CHSV(0, 255, 255); // red
  FastLED.show();
}

#endif
