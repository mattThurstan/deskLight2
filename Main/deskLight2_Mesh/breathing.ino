/*----------------------------Breathing----------------------------*/

/*
 * Breath : (noun) Refers to a full cycle of breathing. It can also refer to the air that is inhaled or exhaled.
 */
 
void loopBreathing() {
  if (_isBreathing == true) {
    breathRiseFall2();
  } else {
    FadeAll(4);
  }
}

uint8_t bBpm = 12;
uint8_t bMax = ( (ledSegment[2].total / 4) - 1 );
uint8_t bPeak;

void breathRiseFall2() {

  if (_isBreathOverlaid == false) {
    strip.ClearTo(_rgbBlack);
  }
  
  bPeak = beatsin8( bBpm, 1, bMax); //bpm, min, max
  
  //fade bot to top
  //ledsLeft( 1, bPeak ).fill_gradient_RGB( CRGB::White, CRGB::Black );

  //strip.SetPixelColor(i, _rgbWhite);
  
  FillGradientRGB(ledSegment[2].first, bPeak, _rgbWhite, _rgbBlack);  // left
  FillGradientRGB((ledSegment[3].first+bPeak), ledSegment[4].total, _rgbBlack, _rgbWhite);  // right
  
}
