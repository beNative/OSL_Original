#ifndef STATES_H
#define STATES_H

#include <Arduino.h>

// ------------------------------------------------------------------------------------------------------------------------------------------------>
const uint16_t NA                   =    -1;                 // For each of the 8 states the light can have the following settings: On, Off, NA, Blink, FastBlink, or Dim. On/Off are defined below
const uint16_t BLINK                =    -2;                 // These give us numerical values to these names which makes coding easier, we can just type in the name instead of the number.
const uint16_t FASTBLINK            =    -3;
const uint16_t SOFTBLINK            =    -4;
const uint16_t DIM                  =    -5;
const uint16_t FADEOFF              =    -6;
const uint16_t XENON                =    -7;
const uint16_t BACKFIRE             =    -8;

const byte ON = 1;
const byte OFF = 0;
const byte YES = 1;
const byte NO = 0;
const byte PRESSED = 0;                                 // Used for buttons pulled-up to Vcc, who are tied to ground when pressed
const byte RELEASED = 1;                                // Used for buttons pulled-up to Vcc, who are tied to ground when pressed

const byte Mode1               =     0;                 // Channel 3 in 1st position
const byte Mode2               =     1;                 // Channel 3 in 2nd position
const byte Mode3               =     2;                 // Channel 3 in 3rd position
const byte Mode4               =     3;                 // Channel 3 in 4th position
const byte Mode5               =     4;                 // Channel 3 in 5th position
const byte StateFwd            =     5;                 // Moving forward
const byte StateRev            =     6;                 // Moving backwards
const byte StateStop           =     7;                 // Stopped
const byte StateStopDelay      =     8;                 // Stopped for a user-defined length of time
const byte StateBrake          =     9;                 // Braking
const byte StateRT             =     10;                // Right turn
const byte StateLT             =     11;                // Left turn
const byte StateAccel          =     12;                // Acceleration
const byte StateDecel          =     13;                // Deceleration

#endif // ndef STATES_H
