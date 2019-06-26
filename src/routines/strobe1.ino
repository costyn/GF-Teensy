#ifdef RT_STROBE1
void strobe1() {
  if ( tapTempo.beatProgress() > 0.95 ) {
#ifdef USING_MPU
    fill_solid(leds, NUM_LEDS, CHSV( map( yprX, 0, 360, 0, 255 ), 255, 255)); // yaw for color
#else
    fill_solid(leds, NUM_LEDS, CHSV( 0, 255, 255));
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
