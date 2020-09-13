/*----------------------------mode---------------------------------*/
/*
 * 0 - glow     (static/moving)
 * 1 - sunrise  (static/moving)
 * 2 - morning  (static/moving)
 * 3 - day      (static/moving) (full bright/overcast/clearbluesky)
 * 4 - working  (static) (full bright) (colour temperature sub-mode)
 * 5 - evening  (static/moving)
 * 6 - sunset   (static/moving)
 * 7 - night    (static/moving)
 * 8 - effect   (moving) (could use static to pause it?)
 * 
 * gradient fade (top/mid/bot) (all can invert) (overlay/fx?)
 * moveable/resizable bright bar (overlay/fx?) (might use brightness +/- intputs to move up/down)
 */

/*
  select mode to run based upon mode var, set by input
  called from main loop
  (select from preset array) ..later
  numbers denote slots
  slots are filled (by hand for now) with named functions
  ..needs an override aswell
  modes are represented as numbers, but have names if needed
  modes are selected individualy or in a present sequence

 */
void loopModes() {
  if (_onOff) {
    if(_modeCur == 0) {       mode0(); } 
    else if(_modeCur == 1) {  mode1(); }
    else if(_modeCur == 2) {  mode2(); }
    else if(_modeCur == 3) {  mode3(); }
    else if(_modeCur == 4) {  mode4(); }        // 'working' - only one in use for now
    else if(_modeCur == 5) {  mode5(); }
    else if(_modeCur == 6) {  mode6(); }
    else if(_modeCur == 7) {  mode7(); }
    else if(_modeCur == 8) {  mode8(); }
  }
  else { }
}

/*----------------------------modes----------------------------*/
void mode0() { /* glow */ 
  strip.ClearTo(_glowColor, ledSegment[1].first, ledSegment[2].last); 
  //for (int i = ledSegment[1].first; i <= ledSegment[3].last; i++) {
  //  strip.SetPixelColor(i, _glowColor);
  //}
}

void mode1() {/* sunrise */ }
  
void mode2() { /* morning */ 
  // this is yellow at bot fade to blue at top.
  strip.ClearTo(_morningColor, ledSegment[1].first, ledSegment[2].last); 
  //for (int i = ledSegment[1].first; i <= ledSegment[3].last; i++) {
  //  strip.SetPixelColor(i, _morningColor);
  //}
  //FillGradientRGB(ledSegment[1].first, ledSegment[2].last, _rgbYellow, _rgbBlue);
} 

void mode3() { /* day */ 
  // this is green at bot fade to blue at top.
  strip.ClearTo(_rgbGreen, ledSegment[1].first, ledSegment[2].last); 
  //for (int i = ledSegment[1].first; i <= ledSegment[3].last; i++) {
  //  strip.SetPixelColor(i, _rgbGreen);
  //}
  //FillGradientRGB(ledSegment[1].first, ledSegment[2].last, _rgbGreen, _rgbBlue);
}

void mode4() { /* working */ 
  for (int i = ledSegment[1].first; i <= ledSegment[3].last; i++) {
    if(_colorTempCur == 0) {
    //strip.SetPixelColor(i, _warmFluorescent);
    strip.ClearTo(_rgbWarmFluorescent, ledSegment[1].first, ledSegment[2].last); 
  } else if(_colorTempCur == 1) {
    //strip.SetPixelColor(i, _standardFluorescent);
    strip.ClearTo(_rgbStandardFluorescent, ledSegment[1].first, ledSegment[2].last); 
  } else if(_colorTempCur == 2) {
    //strip.SetPixelColor(i, _coolWhiteFluorescent);
    strip.ClearTo(_rgbCoolWhiteFluorescent, ledSegment[1].first, ledSegment[2].last); 
  }
  }
}

void mode5() { /* evening */ 
  strip.ClearTo(_eveningColor, ledSegment[1].first, ledSegment[2].last); 
  //for (int i = ledSegment[1].first; i <= ledSegment[3].last; i++) {
  //  strip.SetPixelColor(i, _eveningColor);
  //}
}

void mode6() { /* sunset */ }

void mode7() { /* night */ 
  strip.ClearTo(_nightColor, ledSegment[1].first, ledSegment[2].last); 
  //for (int i = ledSegment[1].first; i <= ledSegment[2].last; i++) {
  //  strip.SetPixelColor(i, _nightColor);
  //}
}
  
void mode8() { /* effect */ 
  strip.ClearTo(_colorHSL, ledSegment[1].first, ledSegment[2].last); 
}
