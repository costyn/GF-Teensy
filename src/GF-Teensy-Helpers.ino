// Fill a gradient on a LED ring with any possible start positions.
// startLed and endLed may be negative (one or both), may be larger than NUM_LEDS (one or both)
// TODO:
// * startLed cannot (yet) be > endLed
// * remove floating point calculation; replace by linear interpolation?

void fillGradientRing( int startLed, CHSV startColor, int endLed, CHSV endColor ) {
  if ( startLed > endLed ) {
    fill_gradient(leds, endLed, CHSV(0, 255, 255) , startLed, CHSV(0, 255, 255), SHORTEST_HUES ); // show RED for error!
  } else {
    // Determine actual start and actual end (normalize using modulo):
    int actualStart = mod(startLed + NUM_LEDS, NUM_LEDS)  ;
    int actualEnd = mod(endLed + NUM_LEDS, NUM_LEDS) ;

    // If beginning is at say 50, and end at 10, then we split the gradient in 2:
    // * one from 50-59
    // * one from 0-10
    // To determine which color should be at 59 and 0 we use the blend function:
    if ( actualStart > actualEnd ) {
      float ratio = 1.0 - float(actualEnd) / float(endLed - startLed) ; // cast to float otherwise the division won't work
      int normalizedRatio = round( ratio * 255 ) ; // determine what ratio of startColor and endColor we need at LED 0
      CHSV colorAtLEDZero = blend(startColor, endColor, normalizedRatio);

      fill_gradient(leds, actualStart, startColor, NUM_LEDS - 1, colorAtLEDZero, SHORTEST_HUES);
      fill_gradient(leds, 0, colorAtLEDZero, actualEnd, endColor, SHORTEST_HUES);
    } else {
      fill_gradient(leds, actualStart, startColor, actualEnd, endColor, SHORTEST_HUES);
    }
  }
}

// This is a little convoluted and could probably be written better :)
void fillSolidRing( int startLed, int endLed, CHSV color ) {
  if ( startLed > endLed ) {
    fill_gradient(leds, endLed, CHSV(0, 255, 255), startLed, CHSV(0, 255, 255), SHORTEST_HUES ); // show RED for error!
  } else {
    // Determine actual start and actual end (normalize using custom modulo):
    int actualStart = mod(startLed + NUM_LEDS, NUM_LEDS)  ;
    int actualEnd = mod(endLed + NUM_LEDS, NUM_LEDS) ;

    // If beginning is at say 50, and end at 10, then we split the fill in 2:
    // * one from 50-59
    // * one from 0-10
    if ( actualStart > actualEnd ) {
      fill_solid(leds + actualStart, NUM_LEDS - actualStart, color);
      fill_solid(leds, actualEnd, color);
    } else {
      fill_solid(leds + actualStart, actualEnd - actualStart, color);
    }
  }
} // end fillSolidRing()


#ifdef USING_MPU

// offset for aligning gyro "bottom" with LED "bottom" - depends on orientation of ring led 0 vs gyro - determine experimentally
#define OFFSET -4

// This routine needs pitch/roll information in floats, so we need to retrieve it separately
//  Suggestions how to fix this/clean it up welcome.
int lowestPoint() {
  Quaternion quat;        // [w, x, y, z]         quaternion container
  VectorFloat gravity;    // [x, y, z]            gravity vector
  float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

  mpu.dmpGetQuaternion(&quat, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &quat);
  mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);

  float myYprP = (ypr[1] * 180 / M_PI) ;  // Convert from radians to degrees:
  float myYprR = (ypr[2] * 180 / M_PI) ;

  int targetLedPos = 0 ;
  static int currentLedPos = 0 ;
  int ratio = 0 ;
  int mySpeed = round( (abs(myYprP) + abs(myYprR) ) * 100 );
  taskLedModeSelect.setInterval( map( mySpeed, 0, 9000, 25, 5) ) ;

  if ( myYprR < 0 and myYprP < 0 ) {
    ratio =  (abs( myYprP ) / (abs(myYprP) + abs(myYprR))) * 100 ;
    targetLedPos = map( ratio, 0, 100, 0 , 14 );

  } else if ( myYprR > 0 and myYprP < 0 ) {
    ratio =  (abs( myYprR ) / (abs(myYprP) + abs(myYprR))) * 100 ;
    targetLedPos = map( ratio, 0, 100, 15 , 29 );

  } else if ( myYprR > 0 and myYprP > 0 ) {
    ratio =  (abs( myYprP ) / (abs(myYprP) + abs(myYprR))) * 100 ;
    targetLedPos = map( ratio, 0, 100, 30 , 44 );

  } else if ( myYprR < 0 and myYprP > 0 ) {
    ratio =  (abs( myYprR ) / (abs(myYprP) + abs(myYprR))) * 100 ;
    targetLedPos = map( ratio, 0, 100, 45 , 60 );
  } else {
    DEBUG_PRINTLN(F("\tWTF\t")) ;  // This should never happen
  }
  targetLedPos = mod(targetLedPos + OFFSET, NUM_LEDS) ;

  if ( currentLedPos != targetLedPos ) {
    bool goClockwise = true ;

    // http://stackoverflow.com/questions/7428718/algorithm-or-formula-for-the-shortest-direction-of-travel-between-two-degrees-on

    if ( mod(targetLedPos - currentLedPos + NUM_LEDS, NUM_LEDS) < NUM_LEDS / 2) {  // custom modulo
      goClockwise = true ;
    } else {
      goClockwise = false  ;
    }

    if ( goClockwise ) {
      currentLedPos++ ;
      if ( currentLedPos > NUM_LEDS - 1 ) {
        currentLedPos = 0 ;
      }
    } else {
      currentLedPos-- ;
      if ( currentLedPos < 0 ) {
        currentLedPos = NUM_LEDS - 1 ;
      }
    }

  }

  return currentLedPos ;
}
#endif

void fadeall(uint8_t fade_all_speed) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(fade_all_speed);
  }
}

void brightall(uint8_t bright_all_speed) {
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] += leds[i].scale8(bright_all_speed) ;
  }
}


void addGlitter( fract8 chanceOfGlitter)
{
  for ( uint8_t i = 0 ; i < 5 ; i++ ) {
    if ( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
  }
}


#define LONG_PRESS_MIN_TIME 500  // minimum time for a long press
#define SHORT_PRESS_MIN_TIME 70   // minimum time for a short press - debounce

#ifdef BUTTON_PIN

void checkButtonPress() {
  static unsigned long buttonTimer = 0;
  static boolean buttonActive = false;

  if (digitalRead(BUTTON_PIN) == LOW) {
    // Start the timer
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }

    // If timer has passed longPressTime, set longPressActive to true
    if ((millis() - buttonTimer > LONG_PRESS_MIN_TIME) && (longPressActive == false)) {
      longPressActive = true;
    }

#ifndef USING_MPU
    if( longPressActive == true ) {
          cycleBrightness() ;
    }
#endif


  } else {
    // Reset when button is no longer pressed
    if (buttonActive == true) {
      buttonActive = false;
      if (longPressActive == true) {
        longPressActive = false;
//        taskLedModeSelect.enableIfNot() ;
      } else {
        if ( millis() - buttonTimer > SHORT_PRESS_MIN_TIME ) {
          ledMode++;
          if (ledMode == NUMROUTINES ) {
            ledMode = 0;
          }

          DEBUG_PRINT(F("ledMode = ")) ;
          DEBUG_PRINT( routines[ledMode] ) ;
          DEBUG_PRINT(F(" mode ")) ;
          DEBUG_PRINTLN( ledMode ) ;

//          FastLED.setBrightness( maxBright ) ; // reset it to 'default'
        }
      }
    }
  }

#ifdef BPM_BUTTON_PIN
  if (digitalRead(BPM_BUTTON_PIN) == LOW) {
    tapTempo.update(true);
//    DEBUG_PRINTLN( tapTempo.getBPM() ) ;
  } else {
    tapTempo.update(false);
  }
#endif

  serialEvent() ;
  if (stringComplete) {
    if( inputString.charAt(0) == 'p' ) {
      inputString.remove(0,1);
      uint8_t input = inputString.toInt();
      if( input < NUMROUTINES ) {
        ledMode = inputString.toInt();
      }
      DEBUG_PRINT("LedMode: ");
      DEBUG_PRINTLN( ledMode ) ;
    }
    if( inputString.charAt(0) == 'b' ) {
      inputString.remove(0,1);
      tapTempo.setBPM(inputString.toInt());
      DEBUG_PRINT("BPM: ");
      DEBUG_PRINTLN( inputString.toInt() ) ;
    }
    if( inputString.charAt(0) == 'm' ) {
      inputString.remove(0,1);
      maxBright = inputString.toInt() ;
      DEBUG_PRINT("MaxBright: ");
      DEBUG_PRINTLN( maxBright ) ;
    }

    // clear the string:
    inputString = "";
    stringComplete = false;
  }

}
#endif

// Only use if no MPU present:
#if !defined(USING_MPU) && defined(BUTTON_PIN)
void cycleBrightness() {
  static uint8_t currentBright = DEFAULT_BRIGHTNESS ;

#if defined(FAN)
  uint8_t deviceMaxBright = 75;
#elif defined(GLOWSTAFF)
  uint8_t deviceMaxBright = 150;
#else
  uint8_t deviceMaxBright = 255;
#endif


  if ( taskCheckButtonPress.getRunCounter() % 100 ) {
    maxBright = lerp8by8( 3, deviceMaxBright, quadwave8( currentBright )) ;
    currentBright++ ;
    DEBUG_PRINTLN( maxBright ) ;
  }

//  taskLedModeSelect.disable() ;
//  fill_solid(leds, NUM_LEDS, CRGB::White );
//  FastLED.show() ;

}
#endif

// Custom mod which always returns a positive number
int mod(int x, int m) {
  return (x % m + m) % m;
}


// for debugging purposes
int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}


/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void autoAdvanceLedMode() {
  ledMode++;
  if (ledMode == NUMROUTINES ) {
    ledMode = 0;
  }
}
