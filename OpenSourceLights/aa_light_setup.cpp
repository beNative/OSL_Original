#include <Arduino.h>
#include "AA_UserConfig.h"
#include "aa_light_setup.h"
#include <avr/pgmspace.h>

// For each of the 8 states the light can have the following settings: On, Off, NA, Blink, FastBlink, or Dim. On/Off are defined below
// These give us numerical values to these names which makes coding easier, we can just type in the name instead of the number.
const uint16_t NA        = -1;
const uint16_t BLINK     = -2;
const uint16_t FASTBLINK = -3;
const uint16_t SOFTBLINK = -4;
const uint16_t DIM       = -5;
const uint16_t FADEOFF   = -6;
const uint16_t XENON     = -7;
const uint16_t BACKFIRE  = -8;
const byte ON        = 1;
const byte OFF       = 0;
const byte YES       = 1;
const byte NO        = 0;

#include "aa_light_settings.h"
