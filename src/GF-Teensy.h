#ifndef GF_TEENSY_H
#define GF_TEENSY_H

#include <FastLED.h>
#include <TaskScheduler.h>
#include <ArduinoTapTempo.h>


void ledModeSelect() ;                          // prototype method
void checkButtonPress() ;                       // prototype method
void autoAdvanceLedMode() ;                       // prototype method
void dmpDataReady() ;
void addGlitter( fract8 chanceOfGlitter) ;
void fillGradientRing( int startLed, CHSV startColor, int endLed, CHSV endColor ) ;
void fillSolidRing( int startLed, int endLed, CHSV color ) ;
void syncToBPM() ;
void getDMPData() ; // prototype method
void ledModeSelect() ;

#endif
