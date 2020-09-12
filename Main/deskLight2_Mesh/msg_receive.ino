/*----------------------------internal mesh messages----------------------------*/
/*---------------------get passed to/from MQTT broker by bridge-----------------*/

/*----------------------------messages - receive----------------------------*/
void receiveMessage(uint32_t from, String msg)
{
  uint8_t firstMsgIndex = msg.indexOf(':');
  String targetSub = msg.substring(0, firstMsgIndex);
  String msgSub = msg.substring(firstMsgIndex+1);

  if (targetSub == "lights/light/switch") {
    if (msgSub == LIGHTS_ON) { _onOff = true; }
    else if (msgSub == LIGHTS_OFF) { _onOff = false; }
    publishState(true);
  } 

  else if (targetSub == "lights/mode") 
  {
    else if (msgSub == "Glow") 
    { _modeCur = 0;
      _modePresetSlotCur = 0; }
    else if (msgSub == "Morning") 
    { _modeCur = 2;
      _modePresetSlotCur = 1; }
    else if (msgSub == "Day") 
    { _modeCur = 3;
      _modePresetSlotCur = 2; }
    else if (msgSub == "Working") 
    { _mdeCur = 4;
      _modePresetSlotCur = 3; }
    else if (msgSub == "Evening") 
    { _modeCur = 5;
      _modePresetSlotCur = 4; }
    else if (msgSub == "Night") 
    { _modeCur = 7;
      _modePresetSlotCur = 5; }
    else if (msgSub == "Effect") 
    { _modeCur = 8; }

//    publishMode(true);                   // bouncing back really badly!!!!
  }
  else if (targetSub == "lights/mode/coltemp") {
    if (msgSub == "Warm") 
    { setColorTemp(0); }
    else if (msgSub == "Standard") 
    { setColorTemp(1); }
    else if (msgSub == "CoolWhite") 
    { setColorTemp(2); }
    
    publishColorTemp(true);
  }
//  else if (targetSub == "lights/mode/effect")
//  {
//    // 
//    //publishEffect(true);
//  }
  else if (targetSub == "lights/brightness/set") {
    uint8_t brightness = msgSub.toInt();
    if (brightness < 0 || brightness > 255) { return; } 
    else {
      setGlobalBrightness(brightness);
      publishBrightness(true);
    }
  }
  else if (targetSub == "lights/hue/cycle/set") {
    uint8_t hueCycleLoopTime = msgSub.toInt();
    if (hueCycleLoopTime < 0 || hueCycleLoopTime > 255) { return; } 
    else {
      _GHue2CycleSaved = hueCycleLoopTime;
      checkAndSetTopGHue2CycleMillis();
      publishGHue2Cycle(true);
    }
  }
  else if (targetSub == "sunrise") {
    // trigger only (global synced)
    if (msgSub == LIGHTS_ON) { /*start sunrise*/ }
    else if (msgSub == LIGHTS_OFF) { /*stop sunrise and revert to previous setting*/ }
  }
  else if (targetSub == "lights/sunrise") {
    // trigger only
    // note: the single mesh msg of 'sunrise' is used for synced global sunrise
    if (msgSub == LIGHTS_ON) { /*start sunrise*/ }
    else if (msgSub == LIGHTS_OFF) { /*stop sunrise and revert to previous setting*/ }
    //else if (msgSub == "receive a time for sunrise to happen at") { /*set sunrise time*/ }
  }
  else if (targetSub == "sunset") {
    // trigger only (global synced)
    if (msgSub == LIGHTS_ON) { /*start sunset*/ }
    else if (msgSub == LIGHTS_OFF) { /*stop sunset and revert to previous setting*/ }
  }
  else if (targetSub == "lights/sunset") {
    // trigger only
    // note: the single mesh msg of 'sunset' is used for synced global sunset
    if (msgSub == LIGHTS_ON) { /*start sunset*/ }
    else if (msgSub == LIGHTS_OFF) { /*stop sunset and revert to previous setting*/ }
    //else if (msgSub == "receive a time for sunset to happen at") { /*set sunset time*/ }
  }
  /*
   * Breath : (noun) Refers to a full cycle of breathing. It can also refer to the air that is inhaled or exhaled.
   */
  else if (targetSub == "breath") {
    // trigger only (global synced)
    // note: the single mesh msg of 'breath' is used for synced global breathing
    if (msgSub == LIGHTS_ON) {
      _topIsBreathingSynced = true;                    // set sync to global
      _topIsBreathing = true;                          // start synced breathing
    }
    else if (msgSub == LIGHTS_OFF) {
      _topIsBreathing = false;                         // stop breathing
      _topIsBreathingSynced = false;                   // set sync to local
    }
    //publishTopBreath(false);
  }
  else if (targetSub == "lights/breath") {
    // trigger only (local)
    // note: the single mesh msg of 'breath' is used for synced global breathing
    if (msgSub == LIGHTS_ON) {
      _isBreathingSynced = false;                   // set sync to local
      _isBreathing = true;                          // start local breathing
    }
    else if (msgSub == LIGHTS_OFF) {
      _isBreathing = false;                         // stop breathing
      _isBreathingSynced = false;                   // set sync to local
    }
    //publishLightsBreath(false);
  }
  else if (targetSub == "lights/breath/xyz") {
    // msg will contain xyz coords for position within the house
    //
    //publishLightsBreathXyzSet(false);
  }
  else if (targetSub == "lights/breath/xyz/mode") {
    // set positional mode
    if (msgSub == "Independent") { }
    else if (msgSub == "Global") { }
    //publishLightsBreathXyzMode(false);
  }
  else if(targetSub == "debug/general/set") {
    if (msgSub == LIGHTS_ON) { DEBUG_GEN = true; } 
    else if (msgSub == LIGHTS_OFF) { DEBUG_GEN = false; }
    publishDebugGeneralState(false);
  }
  else if (targetSub == "debug/overlay/set") {
    if (msgSub == LIGHTS_ON) { DEBUG_OVERLAY = true; }
    else if (msgSub == LIGHTS_OFF) { DEBUG_OVERLAY = false; }
    publishDebugOverlayState(false);
  }
  else if (targetSub == "debug/meshsync/set") {
    if (msgSub == LIGHTS_ON) { DEBUG_MESHSYNC = true; }
    else if (msgSub == LIGHTS_OFF) { DEBUG_MESHSYNC = false; }
    publishDebugMeshsyncState(false);
  }
  else if(targetSub == "debug/comms/set") {
    if (msgSub == LIGHTS_ON) { DEBUG_COMMS = true; } 
    else if (msgSub == LIGHTS_OFF) { DEBUG_COMMS = false; }
    publishDebugCommsState(false);
  }
  // don't really need an ON msg but using just to sure it wasn't sent in error
  else if(targetSub == "debug/reset") { if (msgSub == ON) { doReset(); } }
  else if(targetSub == "debug/restart") 
  {
    uint8_t restartTime = msg.toInt();
    if (restartTime < 0 || restartTime > 255) { return; /* do nothing... */ } 
    else { doRestart(restartTime); }
  }
  else if(targetSub == "reset") { if (msgSub == ON) { doReset(); } }
  else if(targetSub == "restart") 
  {
    uint8_t restartTime = msg.toInt();
    if (restartTime < 0 || restartTime > 255) { return; /* do nothing... */ } 
    else { doRestart(restartTime); }
  }
  else if(targetSub == "lockdown") 
  {
    uint8_t severity = msg.toInt();
    if (severity < 0 || severity > 255) { return; /* do nothing... */ } 
    else { doLockdown(severity); }
  }
  else if(targetSub == "status/request") { if (msgSub == ON) { publishStatusAll(false); }  }
  
  if (DEBUG_COMMS) { Serial.print(targetSub); Serial.print(" : "); Serial.println(msgSub); }
}
