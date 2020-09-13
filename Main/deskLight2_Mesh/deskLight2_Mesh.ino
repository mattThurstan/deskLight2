/*
    'deskLight2_Mesh' by Thurstan. LED strip desk light 2.
    Copyright (C) 2020 MTS Standish (Thurstan|mattKsp)
    
    https://github.com/mattThurstan/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be usefull,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    https://github.com/mattThurstan/
    
    WeMos D1 (R2 &) mini ESP8266, 80 MHz, 115200 baud, 4M, (1M SPIFFS)
    //Wemos D32 Pro (ESP32-WROVER)
*/


/*----------------------------libraries----------------------------*/
#include <MT_LightControlDefines.h>
#include <EEPROM.h>                               // a few saved settings
#include <FastLED.h>                              // still using 'beatsin8' for breathing
#include <NeoPixelBrightnessBus.h>                // NeoPixelBrightnessBus (just for ESP8266)- for brightness functions (instead of NeoPixelBus.h)
#include <painlessMesh.h>                         // https://github.com/gmag11/painlessMesh


/*----------------------------system----------------------------*/
const String _progName = "deskLight2_Mesh";
const String _progVers = "0.302";                 // touch bt

uint8_t LOCKDOWN_SEVERITY = 0;                    // the severity of the lockdown
bool LOCKDOWN = false;                            // are we in lockdown?

boolean DEBUG_GEN = false;                        // realtime serial debugging output - general
boolean DEBUG_OVERLAY = false;                    // show debug overlay on leds (eg. show segment endpoints, center, etc.)
boolean DEBUG_MESHSYNC = false;                   // show painless mesh sync by flashing some leds (no = count of active mesh nodes) 
boolean DEBUG_COMMS = false;                      // realtime serial debugging output - comms
//boolean DEBUG_INTERRUPT = false;                  // realtime serial debugging output - interrupt pins
boolean DEBUG_USERINPUT = false;                  // realtime serial debugging output - user input

boolean _firstTimeSetupDone = false;              // starts false //this is mainly to catch an interrupt trigger that happens during setup, but is usefull for other things
volatile boolean _onOff = true; //flip _state // issues with mqtt and init false // this should init false, then get activated by input - on/off true/false
//bool _dayMode = false;                            // whether or not to run if night or day. default to night just so it works in case something goes wrong.
bool _shouldSaveSettings = false;                 // flag for saving data
bool _runonce = true;                             // flag for sending states when first mesh conection

const int _mainLoopDelay = 0;                     // just in case. Eg. 8 ~= 120 fps (1000/120=8.333)

bool _isBreathing = false;                        // toggle for breath
bool _isBreathOverlaid = false;                   // toggle for whether breath is overlaid on top of modes
bool _isBreathingSynced = false;                  // breath sync local or global

/*----------------------------arduino pins----------------------------*/
// NeoPixelBrightnessBus - For Esp8266, only one Pin and it uses GPIO3 (RX) due to DMA hardware use. 
//const byte _fan0Pin = 1;
//const byte _fan1Pin = ;
const byte _bt0Pin = 16;                          // BT to D0 (GPIO16) with external 10K pullup / touch bt is active low
//const byte _bt0Pin = 0; ???
//const byte _bt1Pin = 2; ???

/*----------------------------modes----------------------------*/
const int _modeNum = 9;                           // ???
const int _modePresetSlotNum = 7;
int _modePreset[_modePresetSlotNum] = { 0, 2, 3, 4, 5, 7, 8 }; // test basic, tap bt to cycle around a few mode slots   //expand to array or struct later for more presets
volatile int _modeCur = 4;                        // current mode in use
int _modePresetSlotCur = 3;                       // the current array pos (slot) in the current preset, as opposed to..      //+/- by userInput
const String _modeName[_modeNum] = { "Glow", "Sunrise", "Morning", "Day", "Working", "Evening", "Sunset", "Night", "Effect" };

const int _colorTempNum = 3;                      // 3 color temperature sub-modes for now
int _colorTempCur = 1;                            // current colour temperature
const String _colorTempName[_colorTempNum] = { "Warm", "Standard", "CoolWhite" }; // color temperature sub-mode names for the main "Working" mode.

/*----------------------------buttons----------------------------*/
//..mabye, but would really like touch due to hardware positioning

/*----------------------------touch sensors----------------------------*/
volatile boolean _bt0Lock = false;                // Button lock
unsigned long _bt0LockSaveTime = 0;               // Save the current time
long _btLockInterval = 1000;                      // Amount of time to lock the button in milliseconds


/*----------------------------LED----------------------------*/
const uint16_t _ledNum = 52;                      // 1 + 51 LEDs
NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(_ledNum);

typedef struct {
  byte first;
  byte last;
  byte total;
} LED_SEGMENT;

const int _segmentTotal = 3;                      // Xm strip with LEDs (3 + 1)
LED_SEGMENT ledSegment[_segmentTotal] = { 
  { 0, 0, 1 },
  { 1, 51, 51 }, 
  { 11, 20, 10 }
};

const int _ledGlobalBrightness = 255;             // global brightness - use this to cap the brightness
uint8_t _ledGlobalBrightnessCur = 255;            // current global brightness - adjust this
uint8_t _ledBrightnessIncDecAmount = 10;          // the brightness amount to increase or decrease
unsigned long _ledRiseSpeed = 30;                 // speed at which the LEDs turn on (runs backwards)
uint8_t _ledRiseSpeedSaved = 30;                  // cos of saving / casting unsigned long issues - use 0-255 via mqtt
uint8_t _gHue2 = 0;                               // incremental cycling "base color", 0-100, converted to 0-1
uint8_t _gHue2saved = 0;                          // used to revert color when going back to 'Normal' mode
unsigned long _gHue2CycleMillis = 200UL;          // gHue loop update time (millis)
uint8_t _gHue2CycleSaved = 100;                   // 0-255 mapped to millis range
uint8_t _gHue2CycleMultiplier = 4;                // (__gHue2CycleSaved * _gHue2CycleMultiplier) = (unsigned long) _gHue2CycleMillis
unsigned long _gHue2PrevMillis;                   // gHue loop previous time (millis)

RgbColor _rgbRed(255, 0, 0);
RgbColor _rgbGreen(0, 255, 0);
RgbColor _rgbBlue(0, 0, 255);
RgbColor _rgbYellow(255, 255, 0);
RgbColor _rgbFuchsia(255, 0, 128);
RgbColor _rgbOrange(255, 165, 0);
RgbColor _rgbViolet(148, 0, 211);
RgbColor _rgbTeal(0, 128, 128);
RgbColor _rgbPink(255, 105, 180);
RgbColor _rgbWhite(255, 250, 255);
RgbColor _rgbGlow(32, 32, 32);
RgbColor _rgbBlack(0, 0, 0);

HslColor _colorHSL(0.25f, 0.5f, 0.5f);

RgbColor _glowColor(16, 16, 16);
RgbColor _morningColor(255, 255, 0);
RgbColor _dayColor(135,206,235);
RgbColor _eveningColor(128, 64, 64);
RgbColor _nightColor(32, 32, 32);

// RGB colours for "Working" colour temperature sub-mode
RgbColor _rgbWarmFluorescent(255, 244, 229);      // WarmFluorescent = 0xFFF4E5 - 0 K, 255, 244, 229
RgbColor _rgbStandardFluorescent(244, 255, 250);  // StandardFluorescent = 0xF4FFFA - 0 K, 244, 255, 250
RgbColor _rgbCoolWhiteFluorescent(212, 235, 255); // CoolWhiteFluorescent = 0xD4EBFF - 0 K, 212, 235, 255
//RgbColor _rgbColorTempCur(_rgbStandardFluorescent); // use this one in day-to-day operations

/*----------------------------Mesh----------------------------*/
painlessMesh  mesh;
uint32_t id_bridge1 = DEVICE_ID_BRIDGE1;

void receivedCallback(uint32_t from, String &msg ) {
  if (DEBUG_GEN && Serial) { Serial.printf("deskLight2_Mesh: Received from %u msg=%s\n", from, msg.c_str()); }
  receiveMessage(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  //if (runonce == true) {
    publishStatusAll(false);
    //runonce = false;
  //}

  if (DEBUG_MESHSYNC) { }
  
  if (DEBUG_COMMS && Serial) { Serial.printf("--> deskLight2_Mesh: New Connection, nodeId = %u\n", nodeId); }
}

void changedConnectionCallback() {
  if (DEBUG_COMMS && Serial) { Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str()); }
  //publish..
  publishStatusAll(false);
  if (DEBUG_MESHSYNC) { }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  if (DEBUG_COMMS && Serial) { Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset); }
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  if (DEBUG_COMMS && Serial) { Serial.printf("Delay to node %u is %d us\n", from, delay); }
}


/*----------------------------MAIN----------------------------*/
void setup() {
  
  // Wemos D1 - GPIO 3 (RX) - swap the pin from serial to a GPIO.
  pinMode(3, FUNCTION_3); // FUNCTION_0 = swap back
  
  // start serial regardless but control debug output from mqtt
  Serial.begin(115200);
  
  Serial.println();
  Serial.print( F("Welcome to Gardening For Nomes part ") );
  Serial.print(_progVers);
  Serial.print( F("!") ); 
  Serial.println();
  Serial.print( F("Your host for todays proceedings is.. ") );
  Serial.print(_progName);
  Serial.println();
  Serial.print("..");
  Serial.println();
  
  delay(3000);                                    // give the power, LED strip, etc. a couple of secs to stabilise
  
  //loadSettings();
  
  setupLEDs();
  flashLED(0);
  
  setupUserInputs();
  flashLED(1);
  
  setupMesh();
  flashLED(2);
  

  //everything done? ok then..
  Serial.print(F("Setup done"));
  Serial.println("-----");
  Serial.print(F("Device Node ID is "));
  String s = String(mesh.getNodeId());
  Serial.println(s);
  Serial.println("-----");
  Serial.println("");

  flashLED(7);
  delay(1500);
  strip.ClearTo(_rgbBlack);
}

void loop() {

  mesh.update();
  
  if(_firstTimeSetupDone == false) {
    _firstTimeSetupDone = true;                   
    publishStatusAll(false);
    if (DEBUG_GEN && Serial) { Serial.print(F("firstTimeSetupDone  = true")); }
  }

  loopUserInputs();
  loopModes();
  loopBreathing();                                // overlaid on top, ..cos
  gHueRotate();
  
  showSegmentEndpoints();                         // overlay debug
  if (DEBUG_MESHSYNC) { }
 
  EVERY_N_SECONDS(60) {                           // too much ???
    if (_shouldSaveSettings == true)
    { 
      //saveSettings(); 
      _shouldSaveSettings = false; 
    }
  }
  //factoryReset();              //TODO             // Press and hold the button to reset to factory defaults

  strip.Show();
  delay(_mainLoopDelay); 
}
