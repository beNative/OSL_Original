#include <Arduino.h>
#include "AA_UserConfig.h"
#include "aa_light_setup.h"
#include <avr/pgmspace.h>

// For each of the 8 states the light can have the following settings: On, Off, NA, Blink, FastBlink, or Dim. On/Off are defined below
// These give us numerical values to these names which makes coding easier, we can just type in the name instead of the number.
const byte NA        = -1;
const byte BLINK     = -2;
const byte FASTBLINK = -3;
const byte SOFTBLINK = -4;
const byte DIM       = -5;
const byte FADEOFF   = -6;
const byte XENON     = -7;
const byte BACKFIRE  = -8;
const byte ON        = 1;
const byte OFF       = 0;
const byte YES       = 1;
const byte NO        = 0;

const byte Schemes[NumSchemes][NumLights][NumStates] PROGMEM =
{
  {
  //    IF CHANNEL 3 is only 3-position switch, values in Pos2 and Pos4 will be ignored (just use Pos1, Pos3, Pos5)
  //    SCHEME 1 - Cruising
  //    Pos 1  Pos 2  Pos 3  Pos 4  Pos 5  Forward  Reverse  Stop  StopDelay  Brake  Right Turn  Left Turn  Accelerating  Decelerating
  // ------------------------------------------------------------------------------------------------------------------------------------------------
    {  OFF,  OFF,   XENON, ON,    ON,    NA,      NA,      NA,   NA,        NA,    NA,         NA,        FASTBLINK,    NA         },  // Light 1  -- Headlight
    {  OFF,  XENON, ON,    ON,    ON,    NA,      NA,      NA,   NA,        NA,    NA,         NA,        NA,           NA         },  // Light 2  -- Fog Light
    {  OFF,  OFF,   DIM,   DIM,   DIM,   NA,      NA,      ON,   NA,        ON,    NA,         NA,        NA,           FASTBLINK  },  // Light 3  -- Break Light
    {  OFF,  OFF,   OFF,   OFF,   OFF,   NA,      NA,      NA,   NA,        NA,    BLINK,      NA,        NA,           NA         },  // Light 4  -- Right Blinker
    {  OFF,  OFF,   OFF,   OFF,   OFF,   NA,      NA,      NA,   NA,        NA,    NA,         BLINK,     NA,           NA         },  // Light 5  -- Left Blinker
    {  OFF,  OFF,   OFF,   OFF,   OFF,   NA,      ON,      NA,   NA,        NA,    NA,         NA,        NA,           NA         },  // Light 6  -- Reverse Lights
    {  OFF,  OFF,   OFF,   OFF,   OFF,   NA,      NA,      NA,   NA,        NA,    NA,         NA,        NA,           NA         },  // Light 7  --
    {  OFF,  OFF,   OFF,   OFF,   ON,    NA,      NA,      NA,   NA,        NA,    NA,         NA,        NA,           NA         },  // Light 8  -- Light Bar
  }
};
