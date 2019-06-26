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

//#define FASTLED_ESP8266_RAW_PIN_ORDER

#include <FastLED.h>
#include <TaskScheduler.h>
#include <ArduinoTapTempo.h>
#include <easing.h>

#ifdef USING_MPU
#include <I2Cdev.h>
#include <MPU6050_6Axis_MotionApps20.h>
#endif

// Uncomment for debug output to Serial. Comment to make small(er) code :)
#define DEBUG

// Uncommment to get MPU debbuging to Serial :
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

#ifdef _TASK_MICRO_RES
#define TASK_RES_MULTIPLIER 1000
#endif

// some patterns which only fill the stips partially can use a little
// brightness boost compared to the ones that light up all LEDs.
// Use sparingly as it'll boost quickly and trip current limiters in power supplies.
#define BRIGHTFACTOR 0.1

#if defined(NEO_PIXEL) || defined(NEO_PIXEL_MULTI)
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB  // Try mixing up the letters (RGB, GBR, BRG, etc) for a whole new world of color combinations
#endif


#if defined(APA_102) || defined(APA_102_SLOW)
#include <SPI.h>
#define CHIPSET     APA102
#define COLOR_ORDER BGR
#endif

// Sanity checks:
#ifndef NUM_LEDS
#error "Error: NUM_LEDS not defined"
#endif

#ifndef DEFAULT_BRIGHTNESS
#error "Error: DEFAULT_BRIGHTNESS not defined"
#endif

#ifndef DEFAULT_BPM
#error "Error: DEFAULT_BPM not defined"
#endif

//#define NUM_LEDS 139
CRGB leds[NUM_LEDS];
uint8_t currentBrightness = DEFAULT_BRIGHTNESS ;

// BPM and button stuff
boolean longPressActive = false;
ArduinoTapTempo tapTempo;

// Serial input to change patterns, speed, etc
String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

// Which mode do we start with
#ifdef DEFAULT_LED_MODE
byte ledMode = DEFAULT_LED_MODE;
#else
byte ledMode = 0;
#endif


/* Scheduler stuff */

#define LEDMODE_SELECT_DEFAULT_INTERVAL  50000   // default scheduling time for LEDMODESELECT, in microseconds
void ledModeSelect() ;                          // prototype method
Scheduler runner;
Task taskLedModeSelect( LEDMODE_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &ledModeSelect);


#ifdef BUTTON_PIN
#define TASK_CHECK_BUTTON_PRESS_INTERVAL  10*TASK_RES_MULTIPLIER   // in milliseconds
void checkButtonPress() ;                       // prototype method
Task taskCheckButtonPress( TASK_CHECK_BUTTON_PRESS_INTERVAL, TASK_FOREVER, &checkButtonPress);
#endif

#ifdef AUTOADVANCE
void autoAdvanceLedMode() ;                       // prototype method
Task taskAutoAdvanceLedMode( 30 * TASK_SECOND, TASK_FOREVER, &autoAdvanceLedMode);
#endif

// ==================================================================== //
// ===               MPU6050 variable declarations                ===== //
// ==================================================================== //


// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#ifdef USING_MPU
MPU6050 mpu;

#define INTERRUPT_PIN 15  // use pin 2 on Arduino Uno & most boards

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

int aaRealX = 0 ;
int aaRealY = 0 ;
int aaRealZ = 0 ;
int yprX = 0 ;
int yprY = 0 ;
int yprZ = 0 ;

void getDMPData() ; // prototype method
Task taskGetDMPData( 3 * TASK_RES_MULTIPLIER, TASK_FOREVER, &getDMPData);
#endif


// #ifdef DEBUG_WITH_TASK
// void printDebugging() ; // prototype method
// Task taskPrintDebugging( 100000, TASK_FOREVER, &printDebugging);
// #endif


// ==================================================================== //
// ============================ Begin Setup =========================== //
// ==================================================================== //


void setup() {
  delay( 1000 ); // power-up safety delay
  #ifdef DEBUG
  Serial.begin(115200);
  #endif

  DEBUG_PRINTLN( F("Starting up...")) ;

  #ifdef NEO_PIXEL
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  DEBUG_PRINTLN( F("Using NEO_PIXEL")) ;
  #endif

  // https://github.com/FastLED/FastLED/wiki/Multiple-Controller-Examples#one-array-many-strips
  #ifdef NEO_PIXEL_MULTI
  FastLED.addLeds<NEOPIXEL, LED_PIN_1>(leds, 0 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, LED_PIN_2>(leds, 1 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, LED_PIN_3>(leds, 2 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, LED_PIN_4>(leds, 3 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  FastLED.addLeds<NEOPIXEL, LED_PIN_5>(leds, 4 * NUM_LEDS_PER_STRIP, NUM_LEDS_PER_STRIP);
  DEBUG_PRINTLN( F("Using NEO_PIXEL_MULTI")) ;
  #endif

  #ifdef APA_102
  FastLED.addLeds<CHIPSET, MY_DATA_PIN, MY_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(12)>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  DEBUG_PRINTLN( F("Using APA_102")) ;
  #endif

  #ifdef APA_102_SLOW
  // Some APA102's require a very low data rate or they start flickering. Shitty quality LEDs? Wiring? Level shifter?? TODO: figure it out!
  FastLED.addLeds<CHIPSET, MY_DATA_PIN, MY_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(2)>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  DEBUG_PRINTLN( F("Using APA_102_SLOW")) ;
  #endif

  FastLED.setBrightness(  currentBrightness );

  #if defined(BUTTON_PIN)
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  DEBUG_PRINT( F("Using BUTTON_PIN: ")) ;
  DEBUG_PRINTLN( BUTTON_PIN );
  #endif

  // On these boards one of the button pins is connected to these, so we pull it low so when the button is pressed, the input pin goes low too.
  #if defined(BUTTON_GND_PIN)
  pinMode(BUTTON_GND_PIN, OUTPUT);
  digitalWrite(BUTTON_GND_PIN, LOW);
  #endif

  #if defined(BUTTON_LED_PIN)
  pinMode(BUTTON_LED_PIN, OUTPUT);
  digitalWrite(BUTTON_LED_PIN, HIGH);
  DEBUG_PRINT( F("Using BUTTON_LED_PIN: ")) ;
  DEBUG_PRINTLN( BUTTON_LED_PIN );
  #endif

  #if defined(BPM_BUTTON_PIN)
  pinMode(BPM_BUTTON_PIN, INPUT_PULLUP);
  DEBUG_PRINT( F("Using BPM_BUTTON_PIN: ")) ;
  DEBUG_PRINTLN( BPM_BUTTON_PIN );
  #endif

  #ifdef AUTOADVANCE
  DEBUG_PRINTLN( F("Using AUTOADVANCE")) ;
  #endif

  // Not sure this actually works
  //FastLED.setMaxPowerInVoltsAndMilliamps(5,475);

  #ifdef ESP8266
    yield(); // not sure if needed to placate ESP watchdog
  #endif

  /* Start the scheduler */
  runner.init();
  runner.addTask(taskLedModeSelect);
  taskLedModeSelect.enable() ;

#ifdef BUTTON_PIN
  runner.addTask(taskCheckButtonPress);
  taskCheckButtonPress.enable() ;
#endif

#ifdef AUTOADVANCE
  runner.addTask(taskAutoAdvanceLedMode);
  taskAutoAdvanceLedMode.enable() ;
#endif


// #ifdef ESP8266
// WiFi.forceSleepBegin();
// #endif

  // ==================================================================== //
  // ============================ MPU Stuff ============================= //
  // ==================================================================== //

#ifdef USING_MPU
  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  DEBUG_PRINTLN( F("Wire library begin done.")) ;

  mpu.initialize();
  DEBUG_PRINTLN( F("MPU initialize done.")) ;
  pinMode(INTERRUPT_PIN, INPUT);
  devStatus = mpu.dmpInitialize();
  DEBUG_PRINTLN( F("DMP initialize done.")) ;

  mpu.setXAccelOffset(X_ACCEL_OFFSET);
  mpu.setYAccelOffset(Y_ACCEL_OFFSET);
  mpu.setZAccelOffset(Z_ACCEL_OFFSET);
  mpu.setXGyroOffset(X_GYRO_OFFSET);
  mpu.setYGyroOffset(Y_GYRO_OFFSET);
  mpu.setZGyroOffset(Z_GYRO_OFFSET);

  DEBUG_PRINTLN( F("MPU calibrate done")) ;

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

  DEBUG_PRINTLN( F("DMP enable done")) ;

  runner.addTask(taskGetDMPData);
  taskGetDMPData.enable() ;
#endif


#ifdef DEBUG_WITH_TASK
  runner.addTask(taskPrintDebugging);
  taskPrintDebugging.enable() ;
#endif

  tapTempo.setBPM(DEFAULT_BPM);
  inputString.reserve(200);
}  // end setup()


// ==================================================================== //
// ============================ Main Loop() =========================== //
// ==================================================================== //


void loop() {
  #ifdef ESP8266
    yield() ; // Pat the ESP watchdog
  #endif
  runner.execute();
}
