/*----------------------------user input----------------------------*/
void setupUserInputs() {
  //setupButtons(); //..leave for pos future use
  setupTouch();
}

/* user inputs loop
 * called from main loop
 */
void loopUserInputs() {
  //loopButtons();
//  loopTouch();
}


/*----------------------------buttons------------------------------*/

/*----------------------------touch sensors-------------------------*/
void setupTouch() {
  pinMode(_bt0Pin, INPUT);                        // Set pin as input (with external 10K pullup)
  //pinMode(_bt1Pin, INPUT);
  //pinMode(_bt0Pin, INPUT_PULLUP);
  //pinMode(_bt1Pin, INPUT_PULLUP);
}

void loopTouch() {
  unsigned long currentTime = millis();           // Get the current time
  
  if (_bt0Lock) {
    if (currentTime - _bt0LockSaveTime > _btLockInterval) {
      _bt0Lock = false;                           // Cancel touch button lock
    }
  } else {
    int val = digitalRead(_bt0Pin);               // Read the input touch pin / touch bt is active low
    if (val == 0) {
      _onOff = !_onOff;
      publishState(true);
      _bt0LockSaveTime = currentTime;             // Save the current time
      _bt0Lock = true;                            // Lock the touch button
      if (DEBUG_USERINPUT && Serial) { Serial.println("Button 0 triggered"); }
    }
  }
}
