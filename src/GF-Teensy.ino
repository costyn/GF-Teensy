/*
   Inspiration from: https://learn.adafruit.com/animated-neopixel-gemma-glow-fur-scarf

   By: Costyn van Dongen

   Note: MAX LEDS: 255 (due to use of uint8_t)

   TODO:
       Get rid of MPU interrupt stuff. I don't need it, but removing it without breaking shit is tricky.
       ripple-pal
       - color rain https://www.youtube.com/watch?v=nHBImYTDZ9I  (for strip, not ring)
*/


// Turn on microsecond resolution; needed to sync some routines to BPM
#define _TASK_MICRO_RES
#define _TASK_TIMECRITICAL
//#define USING_MPU
//#define JELLY
//#define FAN
#define NEWFAN
//#define RING
//#define BALLOON
//#define GLOWSTAFF

#if defined(RING) or defined(NEWFAN)
#define USING_MPU
#endif

#include <FastLED.h>
#include <TaskScheduler.h>
#include <ArduinoTapTempo.h>

#ifdef USING_MPU
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#endif

#define DEFAULT_LED_MODE 0


#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Uncomment for debug output to Serial.
#define DEBUG
//#define DEBUG_WITH_TASK

#ifdef DEBUG
#define DEBUG_PRINT(x)       Serial.print (x)
#define DEBUG_PRINTDEC(x)    Serial.print (x, DEC)
#define DEBUG_PRINTLN(x)     Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

#define TASK_RES_MULTIPLIER 1000

#if defined(RING)
#define DOTSTAR
#else
#define NEO_PIXEL
#endif


//black green white red

#ifdef NEO_PIXEL
#define CHIPSET     WS2812B
#define LED_PIN     17   // which pin your Neopixels are connected to
//#define NUM_LEDS    16   // how many LEDs you have
#define COLOR_ORDER GRB  // Try mixing up the letters (RGB, GBR, BRG, etc) for a whole new world of color combinations
#endif


#ifdef DOTSTAR
#include <SPI.h>
#define CHIPSET     APA102
#define DATA_PIN  11
#define CLOCK_PIN 13
#define COLOR_ORDER BGR
#define NUM_LEDS    144
#endif

#define DEFAULT_BRIGHTNESS 200  // 0-255, higher number is brighter.
#define DEFAULT_BPM 120
#define BUTTON_PIN  16   // button is connected to pin 3 and GND
#define BUTTON_LED_PIN 3   // pin to which the button LED is attached


#ifdef FAN
#define NUM_LEDS 84
#endif

#ifdef RING
#define NUM_LEDS 64
#endif

#ifdef JELLY
#define NUM_LEDS 64
#define BUTTON_PIN 18
#define DEFAULT_BPM 40
#endif

#ifdef GLOWSTAFF
#define LED_PIN     17   // which pin your Neopixels are connected to
#define NUM_LEDS 58
#endif

#ifdef NEWFAN
#define LED_PIN 11
#define NUM_LEDS 72
#define BUTTON_PIN  9   // button is connected to pin 3 and GND
#define BPM_BUTTON_PIN 7  // button for adjusting BPM
#endif




CRGB leds[NUM_LEDS];
uint8_t maxBright = DEFAULT_BRIGHTNESS ;

// BPM and button stuff
boolean longPressActive = false;
ArduinoTapTempo tapTempo;

// Serial input to change patterns, speed, etc
String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete


// A-la-carte routine selection. Uncomment each define below to
// include or not include the routine in the uploaded code.
// Most are kept commented during development for less code to
// and staying within AVR328's flash/ram limits.

#define RT_P_RB_STRIPE
#define RT_P_OCEAN
#define RT_P_HEAT
#define RT_P_LAVA
#define RT_P_PARTY
#define RT_P_FOREST
#define RT_TWIRL1
#define RT_TWIRL2
#define RT_TWIRL4
#define RT_TWIRL6
#define RT_TWIRL2_O
#define RT_TWIRL4_O
#define RT_TWIRL6_O
#define RT_FADE_GLITTER
#define RT_DISCO_GLITTER
//#define RT_RACERS
//#define RT_WAVE
#ifdef USING_MPU
#define RT_SHAKE_IT
#define RT_STROBE1
#define RT_GLED
#endif
//#define RT_HEARTBEAT
#define RT_FASTLOOP
#define RT_FASTLOOP2
#define RT_PENDULUM
#define RT_BOUNCEBLEND
#define RT_JUGGLE_PAL
#define RT_NOISE_LAVA
#define RT_NOISE_PARTY
//#define RT_QUAD_STROBE
//#define RT_PULSE_3
#define RT_PULSE_5_1
#define RT_PULSE_5_2
#define RT_PULSE_5_3
#define RT_THREE_SIN_PAL
//#define RT_BLACK
#define RT_COLOR_GLOW
#ifdef NEWFAN
//#define RT_FAN_WIPE
#endif

byte ledMode = DEFAULT_LED_MODE ; // Which mode do we start with

// Routine Palette Rainbow is always included - a safe routine
const char *routines[] = {
  "p_rb",
#ifdef RT_P_RB_STRIPE
  "p_rb_stripe",
#endif
#ifdef RT_P_OCEAN
  "p_ocean",
#endif
#ifdef RT_P_HEAT
  "p_heat",
#endif
#ifdef RT_P_LAVA
  "p_lava",
#endif
#ifdef RT_P_PARTY
  "p_party",
#endif
#ifdef RT_TWIRL1
  "twirl1",
#endif
#ifdef RT_TWIRL2
  "twirl2",
#endif
#ifdef RT_TWIRL4
  "twirl4",
#endif
#ifdef RT_TWIRL6
  "twirl6",
#endif
#ifdef RT_TWIRL2_O
  "twirl2o",
#endif
#ifdef RT_TWIRL4_O
  "twirl4o",
#endif
#ifdef RT_TWIRL6_O
  "twirl6o",
#endif
#ifdef RT_FADE_GLITTER
  "fglitter",
#endif
#ifdef RT_DISCO_GLITTER
  "dglitter",
#endif
#ifdef RT_RACERS
  "racers",
#endif
#ifdef RT_WAVE
  "wave",
#endif
#ifdef RT_SHAKE_IT
  "shakeit",
#endif
#ifdef RT_STROBE1
  "strobe1",
#endif
#ifdef RT_STROBE2
  "strobe2",
#endif
#ifdef RT_GLED
  "gled",
#endif
#ifdef RT_HEARTBEAT
  "heartbeat",
#endif
#ifdef RT_FASTLOOP
  "fastloop",
#endif
#ifdef RT_FASTLOOP2
  "fastloop2",
#endif
#ifdef RT_PENDULUM
  "pendulum",
#endif
#ifdef RT_VUMETER
  "vumeter",
#endif
#ifdef RT_NOISE_LAVA
  "noise_lava",
#endif
#ifdef RT_NOISE_PARTY
  "noise_party",
#endif
#ifdef RT_BOUNCEBLEND
  "bounceblend",
#endif
#ifdef RT_JUGGLE_PAL
  "jugglepal",
#endif
#ifdef RT_QUAD_STROBE
  "quadstrobe",
#endif
#ifdef RT_PULSE_3
  "pulse3",
#endif
#ifdef RT_PULSE_5_1
  "pulse5_1",
#endif
#ifdef RT_PULSE_5_2
  "pulse5_2",
#endif
#ifdef RT_PULSE_5_3
  "pulse5_3",
#endif
#ifdef RT_THREE_SIN_PAL
  "tsp",
#endif
#ifdef RT_COLOR_GLOW
  "color_glow",
#endif
#ifdef RT_FAN_WIPE
  "fan_wipe",
#endif
#ifdef RT_BLACK
  "black",
#endif

};

#define NUMROUTINES (sizeof(routines)/sizeof(char *)) //array size


/* Scheduler stuff */

#define LEDMODE_SELECT_DEFAULT_INTERVAL  50000   // default scheduling time for LEDMODESELECT, in microseconds
void ledModeSelect() ;                          // prototype method
Scheduler runner;
Task taskLedModeSelect( LEDMODE_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &ledModeSelect);


#define TASK_CHECK_BUTTON_PRESS_INTERVAL  10*TASK_RES_MULTIPLIER   // in milliseconds
void checkButtonPress() ;                       // prototype method
Task taskCheckButtonPress( TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPress);


// ==================================================================== //
// ===                      MPU6050 stuff                         ===== //
// ==================================================================== //


// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#ifdef USING_MPU
MPU6050 mpu;

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

/*
  void dmpDataReady() ;
  void addGlitter( fract8 chanceOfGlitter) ;
  int mod(int x, int m) ;
  int activityLevel();
  bool isMpuUp();
  bool isMpuDown();
  void fillGradientRing( int startLed, CHSV startColor, int endLed, CHSV endColor ) ;
  void fillSolidRing( int startLed, int endLed, CHSV color ) ;
  void syncToBPM() ;
*/

int aaRealX = 0 ;
int aaRealY = 0 ;
int aaRealZ = 0 ;
int yprX = 0 ;
int yprY = 0 ;
int yprZ = 0 ;

void getDMPData() ; // prototype method
Task taskGetDMPData( 3 * TASK_RES_MULTIPLIER, TASK_FOREVER, &getDMPData);
#endif


#ifdef DEBUG_WITH_TASK
void printDebugging() ; // prototype method
Task taskPrintDebugging( 100000, TASK_FOREVER, &printDebugging);
#endif




void setup() {
  delay( 1000 ); // power-up safety delay
#ifdef NEO_PIXEL
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
#endif
#ifdef DOTSTAR
  FastLED.addLeds<CHIPSET, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
#endif
  FastLED.setBrightness(  maxBright );

//  FastLED.setDither( 0 );

  pinMode(BUTTON_PIN, INPUT);
#ifdef BALLOON
  pinMode(15, OUTPUT);
  digitalWrite(15, LOW);
#endif
  pinMode(BUTTON_LED_PIN, OUTPUT);
  digitalWrite(BUTTON_LED_PIN, HIGH);

  pinMode(BPM_BUTTON_PIN, INPUT_PULLUP);

  set_max_power_in_volts_and_milliamps(5, 500);

#ifdef DEBUG
  Serial.begin(115200) ;
  DEBUG_PRINT( F("Starting up. Numroutines = ")) ;
  DEBUG_PRINTLN( NUMROUTINES ) ;

#endif

  /* Start the scheduler */
  runner.init();
  runner.addTask(taskLedModeSelect);
  taskLedModeSelect.enable() ;

  runner.addTask(taskCheckButtonPress);
  taskCheckButtonPress.enable() ;


  // ==================================================================== //
  // ==================================================================== //

#ifdef USING_MPU
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  mpu.initialize();
  devStatus = mpu.dmpInitialize();

#ifdef RING
// Strip MPU
// Your offsets:  -1527 -1127 2339  51  4 -7

  mpu.setXAccelOffset(-1527 );
  mpu.setYAccelOffset(-1127);
  mpu.setZAccelOffset(2339);
  mpu.setXGyroOffset(51);
  mpu.setYGyroOffset(4);
  mpu.setZGyroOffset(-7);
#endif

#ifdef NEWFAN
//  Your offsets:	410	-255	1745	-114	19	-23
// Data is printed as: acelX acelY acelZ giroX giroY giroZ

  mpu.setXAccelOffset(410 );
  mpu.setYAccelOffset(-255);
  mpu.setZAccelOffset(1745);
  mpu.setXGyroOffset(-114);
  mpu.setYGyroOffset(19);
  mpu.setZGyroOffset(-23);
#endif

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();

  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    DEBUG_PRINT(F("DMP Initialization failed (code "));
    DEBUG_PRINT(devStatus);
    DEBUG_PRINTLN(F(")"));
  }

  runner.addTask(taskGetDMPData);
  taskGetDMPData.enable() ;
#endif


#ifdef DEBUG_WITH_TASK
  runner.addTask(taskPrintDebugging);
  taskPrintDebugging.enable() ;
#endif

  tapTempo.setMaxBPM(DEFAULT_BPM);
  inputString.reserve(200);
}  // end setup()




void loop() {
  runner.execute();
}




void ledModeSelect() {

  if ( strcmp(routines[ledMode], "p_rb") == 0  ) {
    FillLEDsFromPaletteColors(0) ;

#ifdef RT_P_RB_STRIPE
  } else if ( strcmp(routines[ledMode], "p_rb_stripe") == 0 ) {
    FillLEDsFromPaletteColors(1) ;
#endif

#ifdef RT_P_OCEAN
  } else if ( strcmp(routines[ledMode], "p_ocean") == 0 ) {
    FillLEDsFromPaletteColors(2) ;
#endif

#ifdef RT_P_HEAT
  } else if ( strcmp(routines[ledMode], "p_heat") == 0 ) {
    FillLEDsFromPaletteColors(3) ;
#endif

#ifdef RT_P_LAVA
  } else if ( strcmp(routines[ledMode], "p_lava") == 0 ) {
    FillLEDsFromPaletteColors(4) ;
#endif

#ifdef RT_P_PARTY
  } else if ( strcmp(routines[ledMode], "p_party") == 0 ) {
    FillLEDsFromPaletteColors(5) ;
#endif

#ifdef RT_P_CLOUD
  } else if ( strcmp(routines[ledMode], "p_cloud") == 0 ) {
    FillLEDsFromPaletteColors(6) ;
#endif

#ifdef RT_P_FOREST
  } else if ( strcmp(routines[ledMode], "p_forest") == 0 ) {
    FillLEDsFromPaletteColors(7) ;
#endif

#ifdef RT_TWIRL1
  } else if ( strcmp(routines[ledMode], "twirl1") == 0 ) {
    twirlers( 1, false ) ;
    taskLedModeSelect.setInterval( TASK_IMMEDIATE ) ;
#endif

#ifdef RT_TWIRL2
  } else if ( strcmp(routines[ledMode], "twirl2") == 0 ) {
    twirlers( 2, false ) ;
#endif

#ifdef RT_TWIRL4
  } else if ( strcmp(routines[ledMode], "twirl4") == 0 ) {
    twirlers( 4, false ) ;
#endif

#ifdef RT_TWIRL6
  } else if ( strcmp(routines[ledMode], "twirl6") == 0 ) {
    twirlers( 6, false ) ;
#endif

#ifdef RT_TWIRL2_O
  } else if ( strcmp(routines[ledMode], "twirl2o") == 0 ) {
    twirlers( 2, true ) ;
#endif

#ifdef RT_TWIRL4_O
  } else if ( strcmp(routines[ledMode], "twirl4o") == 0 ) {
    twirlers( 4, true ) ;
#endif

#ifdef RT_TWIRL6_O
  } else if ( strcmp(routines[ledMode], "twirl6o") == 0 ) {
    twirlers( 6, true ) ;
#endif

#ifdef RT_FADE_GLITTER
  } else if ( strcmp(routines[ledMode], "fglitter") == 0 ) {
    fadeGlitter() ;
    //taskLedModeSelect.setInterval( map( constrain( activityLevel(), 0, 4000), 0, 4000, 20, 5 ) * TASK_RES_MULTIPLIER ) ;
#ifdef USING_MPU
    taskLedModeSelect.setInterval( map( constrain( activityLevel(), 0, 2500), 0, 2500, 40, 2 ) * TASK_RES_MULTIPLIER ) ;
#else
    taskLedModeSelect.setInterval( 20 * TASK_RES_MULTIPLIER ) ;
#endif
#endif

#ifdef RT_DISCO_GLITTER
  } else if ( strcmp(routines[ledMode], "dglitter") == 0 ) {
    discoGlitter() ;
#ifdef USING_MPU
    taskLedModeSelect.setInterval( map( constrain( activityLevel(), 0, 2500), 0, 2500, 40, 2 ) * TASK_RES_MULTIPLIER ) ;
#else
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif
#endif

#ifdef RT_GLED
    // Gravity LED
  } else if ( strcmp(routines[ledMode], "gled") == 0 ) {
    gLed() ;
    taskLedModeSelect.setInterval( 5 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_BLACK
  } else if ( strcmp(routines[ledMode], "black") == 0 ) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    taskLedModeSelect.setInterval( 500 * TASK_RES_MULTIPLIER ) ;  // long because nothing is going on anyways.
#endif

#ifdef RT_RACERS
  } else if ( strcmp(routines[ledMode], "racers") == 0 ) {
    racingLeds() ;
    taskLedModeSelect.setInterval( 8 * TASK_RES_MULTIPLIER) ;
#endif

#ifdef RT_WAVE
  } else if ( strcmp(routines[ledMode], "wave") == 0 ) {
    waveYourArms() ;
    taskLedModeSelect.setInterval( 15 * TASK_RES_MULTIPLIER) ;
#endif

#ifdef RT_SHAKE_IT
  } else if ( strcmp(routines[ledMode], "shakeit") == 0 ) {
    shakeIt() ;
    taskLedModeSelect.setInterval( 8 * 1000 ) ;
#endif

#ifdef RT_STROBE1
  } else if ( strcmp(routines[ledMode], "strobe1") == 0 ) {
    strobe1() ;
    taskLedModeSelect.setInterval( 5 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_STROBE2
  } else if ( strcmp(routines[ledMode], "strobe2") == 0 ) {
    strobe2() ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_HEARTBEAT
  } else if ( strcmp(routines[ledMode], "heartbeat") == 0 ) {
    heartbeat() ;
    taskLedModeSelect.setInterval( 5 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_VUMETER
  } else if ( strcmp(routines[ledMode], "vumeter") == 0 ) {
    vuMeter() ;
    taskLedModeSelect.setInterval( 8 * TASK_RES_MULTIPLIER) ;
#endif

#ifdef RT_FASTLOOP
  } else if ( strcmp(routines[ledMode], "fastloop") == 0 ) {
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER) ;
    fastLoop( false ) ;
#endif

#ifdef RT_FASTLOOP2
  } else if ( strcmp(routines[ledMode], "fastloop2") == 0 ) {
    fastLoop( true ) ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER) ;
#endif

#ifdef RT_PENDULUM
  } else if ( strcmp(routines[ledMode], "pendulum") == 0 ) {
    pendulum() ;
    taskLedModeSelect.setInterval( 1500 ) ; // needs a fast refresh rate
#endif

#ifdef RT_BOUNCEBLEND
  } else if ( strcmp(routines[ledMode], "bounceblend") == 0 ) {
    bounceBlend() ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_JUGGLE_PAL
  } else if ( strcmp(routines[ledMode], "jugglepal") == 0 ) {
    jugglePal() ;
    taskLedModeSelect.setInterval( 150 ) ; // fast refresh rate needed to not skip any LEDs
#endif

#ifdef RT_NOISE_LAVA
  } else if ( strcmp(routines[ledMode], "noise_lava") == 0 ) {
    fillnoise8( 0, beatsin8( tapTempo.getBPM(), 1, 25), 30, 1); // pallette, speed, scale, loop
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_NOISE_PARTY
  } else if ( strcmp(routines[ledMode], "noise_party") == 0 ) {
    fillnoise8( 1, beatsin8( tapTempo.getBPM(), 1, 25), 30, 1); // pallette, speed, scale, loop
    //    taskLedModeSelect.setInterval( beatsin16( tapTempo.getBPM(), 2000, 50000) ) ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_NOISE_OCEAN
  } else if ( strcmp(routines[ledMode], "noise_ocean") == 0 ) {
    fillnoise8( 2, beatsin8( tapTempo.getBPM(), 1, 25), 30, 1); // pallette, speed, scale, loop
    //    taskLedModeSelect.setInterval( beatsin16( tapTempo.getBPM(), 2000, 50000) ) ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif


#ifdef RT_QUAD_STROBE
  } else if ( strcmp(routines[ledMode], "quadstrobe") == 0 ) {
    quadStrobe();
    taskLedModeSelect.setInterval( (60000 / (tapTempo.getBPM() * 4)) * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_PULSE_3
  } else if ( strcmp(routines[ledMode], "pulse3") == 0 ) {
    pulse3();
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_PULSE_5_1
  } else if ( strcmp(routines[ledMode], "pulse5_1") == 0 ) {
    pulse5(1, true);
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_PULSE_5_2
  } else if ( strcmp(routines[ledMode], "pulse5_2") == 0 ) {
    pulse5(2, true);
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_PULSE_5_3
  } else if ( strcmp(routines[ledMode], "pulse5_3") == 0 ) {
    pulse5(3, true);
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_THREE_SIN_PAL
  } else if ( strcmp(routines[ledMode], "tsp") == 0 ) {
    threeSinPal() ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_COLOR_GLOW
  } else if ( strcmp(routines[ledMode], "color_glow") == 0 ) {
    colorGlow() ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_FAN_WIPE
  } else if ( strcmp(routines[ledMode], "fan_wipe") == 0 ) {
    fanWipe() ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif
  }

}
