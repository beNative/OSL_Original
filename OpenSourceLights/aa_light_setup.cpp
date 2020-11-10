#include <Arduino.h>
#include "AA_UserConfig.h"
#include "aa_light_setup.h"
#include <avr/pgmspace.h>

// For each of the 8 states the light can have the following settings:
// ON, OFF, NA, BLINK, FASTBLINK, or DIM. OF/OFF are defined below
// These give us numerical values to these names which makes coding easier,
// we can just type in the name instead of the number.
const int8_t NA        = -1;
const int8_t BLINK     = -2;
const int8_t FASTBLINK = -3;
const int8_t SOFTBLINK = -4;
const int8_t DIM       = -5;
const int8_t FADEOFF   = -6;
const int8_t XENON     = -7;
const int8_t BACKFIRE  = -8;

const int8_t ON  = 1;
const int8_t OFF = 0;

const int8_t YES = 1;
const int8_t NO  = 0;

#include "aa_light_settings.h"