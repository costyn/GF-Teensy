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
#ifdef RT_FIRE2012
  "fire2012",
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
#ifdef RT_FASTLOOP3
  "fastloop3",
#endif
#ifdef RT_FASTLOOP4
  "fastloop4",
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
#ifdef RT_DROPLETS
  "droplets",
#endif
#ifdef RT_DROPLETS2
  "droplets2",
#endif
#ifdef RT_BOUNCYBALLS
  "bouncyballs",
#endif
#ifdef RT_CIRC_LOADER1
  "circloader1",
#endif
#ifdef RT_CIRC_LOADER3
  "circloader3",
#endif
#ifdef RT_RIPPLE
  "ripple",
#endif
#ifdef RT_RANDOMWALK
  "randomwalk",
#endif
#ifdef RT_REDDOTS1
  "reddots1",
#endif
#ifdef RT_REDDOTS2
  "reddots2",
#endif
#ifdef RT_REDDOTS3
  "reddots3",
#endif

#ifdef RT_POVPATTERNS
  "povpatterns",
#endif
#ifdef RT_BLACK
  "black",
#endif

};

#define NUMROUTINES (sizeof(routines)/sizeof(char *)) //array size


void ledModeSelect() {
  #ifdef ESP8266
    yield();
  #endif

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
    taskLedModeSelect.setInterval( TASK_IMMEDIATE ) ;
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

#ifdef RT_FIRE2012
  } else if ( strcmp(routines[ledMode], "fire2012") == 0 ) {
    Fire2012() ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
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
    fastLoop( false, false ) ;
#endif

#ifdef RT_FASTLOOP2
  } else if ( strcmp(routines[ledMode], "fastloop2") == 0 ) {
    fastLoop( true, false ) ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER) ;
#endif

#ifdef RT_FASTLOOP3
} else if ( strcmp(routines[ledMode], "fastloop3") == 0 ) {
    fastLoop3() ;
    taskLedModeSelect.setInterval( 15 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_FASTLOOP4
} else if ( strcmp(routines[ledMode], "fastloop4") == 0 ) {
    fastLoop( true, true ) ;
    taskLedModeSelect.setInterval( 5 * TASK_RES_MULTIPLIER ) ;
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
    if( tapTempo.getBPM() > 50 ) {
      fillnoise8( 0, beatsin8( tapTempo.getBPM(), 1, 25), 30, 1); // pallette, speed, scale, loop
    } else {
      fillnoise8( 0, 1, 30, 1); // pallette, speed, scale, loop
    }
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_NOISE_PARTY
  } else if ( strcmp(routines[ledMode], "noise_party") == 0 ) {
    if( tapTempo.getBPM() > 50 ) {
      fillnoise8( 1, beatsin8( tapTempo.getBPM(), 1, 25), 30, 1); // pallette, speed, scale, loop
    } else {
      fillnoise8( 1, 1, 30, 1); // pallette, speed, scale, loop
    }
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_NOISE_OCEAN
  } else if ( strcmp(routines[ledMode], "noise_ocean") == 0 ) {
    fillnoise8( 2, beatsin8( tapTempo.getBPM(), 1, 25), 30, 1); // pallette, speed, scale, loop
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

#ifdef RT_DROPLETS
} else if ( strcmp(routines[ledMode], "droplets") == 0 ) {
    droplets() ;
    taskLedModeSelect.setInterval( 30 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_BOUNCYBALLS
} else if ( strcmp(routines[ledMode], "bouncyballs") == 0 ) {
    bouncyBalls() ;
    taskLedModeSelect.setInterval( 30 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_DROPLETS2
} else if ( strcmp(routines[ledMode], "droplets2") == 0 ) {
    droplets2() ;
    taskLedModeSelect.setInterval( 10 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_CIRC_LOADER1
} else if ( strcmp(routines[ledMode], "circloader1") == 0 ) {
    circularLoader1() ;
    taskLedModeSelect.setInterval( 50 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_CIRC_LOADER3
} else if ( strcmp(routines[ledMode], "circloader3") == 0 ) {
    circularLoader3() ;
    taskLedModeSelect.setInterval( 5 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_RIPPLE
} else if ( strcmp(routines[ledMode], "ripple") == 0 ) {
    ripple() ;
    taskLedModeSelect.setInterval( 100 * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_RANDOMWALK
} else if ( strcmp(routines[ledMode], "randomwalk") == 0 ) {
    randomWalk() ;
    taskLedModeSelect.setInterval( 5 * TASK_RES_MULTIPLIER ) ;
#endif


#ifdef RT_REDDOTS1
} else if ( strcmp(routines[ledMode], "reddots1") == 0 ) {
    redDots1() ;
    taskLedModeSelect.setInterval( TASK_IMMEDIATE ) ;
#endif

#ifdef RT_REDDOTS2
} else if ( strcmp(routines[ledMode], "reddots2") == 0 ) {
    redDots2() ;
    taskLedModeSelect.setInterval( beatsin8(60,5,15) * TASK_RES_MULTIPLIER ) ;
#endif

#ifdef RT_REDDOTS3
} else if ( strcmp(routines[ledMode], "reddots3") == 0 ) {
    redDots3() ;
    taskLedModeSelect.setInterval( 40 * TASK_RES_MULTIPLIER ) ;
#endif


#ifdef RT_POVPATTERNS
} else if ( strcmp(routines[ledMode], "povpatterns") == 0 ) {
  #define width(array) sizeof(array) / sizeof(array[0])
  DEBUG_PRINTLN( "." ) ;

    povPatterns(30, abstract1, width(abstract1));
    taskLedModeSelect.setInterval( 30000 ) ; // microseconds ; fast needed for POV patterns
#endif

#ifdef RT_BLACK
  } else if ( strcmp(routines[ledMode], "black") == 0 ) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    taskLedModeSelect.setInterval( 500 * TASK_RES_MULTIPLIER ) ;  // long because nothing is going on anyways.
#endif

  }
}
