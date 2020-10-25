#include <Arduino.h>
#include "AA_UserConfig.h"
#include "aa_light_setup.h"
#include <avr/pgmspace.h>

// For each of the 8 states the light can have the following settings: On, Off, NA, Blink, FastBlink, or Dim. On/Off are defined below
// These give us numerical values to these names which makes coding easier, we can just type in the name instead of the number.
const int NA        = -1;
const int BLINK     = -2;
const int FASTBLINK = -3;
const int SOFTBLINK = -4;
const int DIM       = -5;
const int FADEOFF   = -6;
const int XENON     = -7;
const int BACKFIRE  = -8;

const byte ON  = 1;
const byte OFF = 0;

const byte YES = 1;
const byte NO  = 0;

#include "aa_light_settings.h"