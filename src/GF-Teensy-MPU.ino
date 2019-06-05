// ================================================================
// ================================================================
// =====             MPU 6050 CODE STUFF                      =====
// ================================================================
// ================================================================

#ifdef USING_MPU

// ===               INTERRUPT DETECTION ROUTINE               ===
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
  DEBUG_PRINTLN(F(":"));
}



void getDMPData() {

  if ( fifoCount > packetSize ) {
    DEBUG_PRINTLN(F("yo!"));
    mpu.resetFIFO();
  }

  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();

  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    mpu.resetFIFO();
    DEBUG_PRINTLN(F("FIFO overflow!"));

  } else if (mpuIntStatus & 0x02) {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
  }

  getYPRAccel() ;
}

// display Euler angles in degrees
void getYPRAccel() {
  // orientation/motion vars
  Quaternion quat;        // [w, x, y, z]         quaternion container
  VectorInt16 aa;         // [x, y, z]            accel sensor measurements
  VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
  VectorFloat gravity;    // [x, y, z]            gravity vector
  float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

  mpu.dmpGetQuaternion(&quat, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &quat);
  mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);
  mpu.dmpGetAccel(&aa, fifoBuffer);
  mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);

  yprX = (ypr[0] * 180 / M_PI) + 180;
  yprY = (ypr[1] * 180 / M_PI) + 90;
  yprZ = (ypr[2] * 180 / M_PI) + 90;

  aaRealX = aaReal.x ;
  aaRealY = aaReal.y ;
  aaRealZ = aaReal.z ;

  bool isVertical   = ( abs( ypr[1] * 180 / M_PI ) + abs( ypr[2] * 180 / M_PI ) > 65.0 ) ;

  int16_t maxXY    = max( aaReal.x, aaReal.y) ;
  int16_t maxAccel = max( maxXY, aaReal.z) ;

  static bool inBeat = false; // debounce variable - only update tapTempo once during high acceleration
  static bool firstPress = true ;
  static uint16_t initialYaw ;

  // only count BPM when button is held down
  if ( isVertical and longPressActive ) {
    if ( maxAccel > 7000 and inBeat == false ) {
      inBeat = true ;
      tapTempo.update(true);
      digitalWrite(BUTTON_LED_PIN, HIGH); // Beat detected, light up to show beat detected
      // DEBUG_PRINTLN("BEAT detected");
    } else if ( maxAccel < 7000 ) {
      inBeat = false ;
      tapTempo.update(false);
      digitalWrite(BUTTON_LED_PIN, LOW); // turn off LED, normal operation
    }
  } else if ( ! isVertical and longPressActive ) {
    if ( firstPress ) { // check if this is the first iteration where button is pressed, remember yaw.
      initialYaw = yprX ;
      firstPress = false ;
    }

    if ( taskGetDMPData.getRunCounter() % 30 == 0 ) { // fast blinking to indicate brightness adjustment process
      digitalWrite(BUTTON_LED_PIN, HIGH);
    } else {
      digitalWrite(BUTTON_LED_PIN, LOW);
    }

    #define RANGE 90
    int conYprX = constrain( yprX, initialYaw - RANGE, initialYaw + RANGE) ;
      currentBrightness = map( conYprX, initialYaw - RANGE, initialYaw + RANGE, 1, 255) ;
     // DEBUG_PRINT(F("\t"));
     // DEBUG_PRINT(yprX);
     // DEBUG_PRINT(F("\t"));
     // DEBUG_PRINT(initialYaw);
     // DEBUG_PRINT(F("\t"));
     // DEBUG_PRINT(conYprX);
     // DEBUG_PRINT(F("\t"));
     // DEBUG_PRINT(currentBrightness);
     // DEBUG_PRINT(F("\t"));
     // DEBUG_PRINTLN();
  }

  if ( ! longPressActive ) {
    tapTempo.update(false);  // needs to be here even though we are not measuring beats

    if ( tapTempo.beatProgress() > 0.95 ) {
      digitalWrite(BUTTON_LED_PIN, HIGH); // turn on LED, normal operation
    } else {
      digitalWrite(BUTTON_LED_PIN, LOW); // turn on LED, normal operation
    }

    firstPress = true ;
  }
}



int activityLevel() {
  return round( (abs( aaRealX )  + abs( aaRealY )  + abs( aaRealZ )) / 3 );
}



bool isTilted() {
#define TILTED_AT_DEGREES 10
  return ( 90 - TILTED_AT_DEGREES > max(yprY, yprZ) or yprY > 90 + TILTED_AT_DEGREES ) ;
}

// check if MPU is pitched up

bool isMpuUp() {
  return yprZ < 90 ;
}

// check if MPU is pitched down
bool isMpuDown() {
  return yprZ > 90 ;
}


bool isYawReliable() {
#define MAXANGLE 45
  // Check if yprY or yprZ are tilted more than MAXANGLE. 90 = level
  // yaw is not reliable below MAXANGLE
  return ( MAXANGLE < min(yprY, yprZ) and max(yprY, yprZ) < 90 + MAXANGLE ) ;
}

#endif


#ifdef DEBUG_WITH_TASK
void printDebugging() {
    // DEBUG_PRINT( taskLedModeSelect.getRunCounter() );
    // DEBUG_PRINT( "\t" );
    // DEBUG_PRINT( taskLedModeSelect.getInterval() );
    // DEBUG_PRINT( "\t" );
    // DEBUG_PRINT( taskLedModeSelect.getStartDelay() );

  DEBUG_PRINT(yprX);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(yprY);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(yprZ);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(aaRealX);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(aaRealY);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(aaRealZ);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(activityLevel());
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(isYawReliable());
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT( taskGetDMPData.getRunCounter() ) ;
  // DEBUG_PRINT("\t");
  // DEBUG_PRINT( freeRam() ) ;

   DEBUG_PRINTLN() ;
  }
#endif
