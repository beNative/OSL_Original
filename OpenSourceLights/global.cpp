  #include <Arduino.h>
  #include "global.h"
  #include "aa_light_setup.h"
  #include "AA_UserConfig.h"

  int MaxFwdSpeed = 100;
  int MaxRevSpeed = -100;
  int MaxRightTurn = 100;
  int MaxLeftTurn = -100;
  bool Failsafe = false;                 // If we loose contact with the Rx this flag becomes True
  unsigned long runCount = 0;

  // CHANGE-SCHEME-MODE MENU VARIABLES
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  bool canChangeScheme     = false;                    // Are we allowed to enter Change Scheme Mode? (This is set to true after being in the STOP state for several seconds)
  unsigned int BlinkOffID;                                // SimpleTimer ID number for the blinking lights
  bool Blinking;                                // Are the lights blinking?
  bool State;                                   // If blinking, are they blinking on (true) or off (false)?
  bool PriorState;                              // Blinking state in the prior iteration
  int TimesBlinked;                                // How many times have the lights blinked
  bool ChangeSchemeMode = false;                // A flag to indicate if we are in Change-Scheme-Mode or not

  // NEW LIGHT FUNCTIONS - Wombii
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Calculate loopsPerCycle: each loop is probably 5-20ms depending on radio, so 50 *20ms = 1 second.
  const byte blinkLoopsPerCycle       = 40;
  const byte blinkLoopsOn             = 20;

  const byte softblinkLoopsPerCycle   = 100;
  const byte softblinkLoopsOn         = 40;

  const byte fastblinkLoopsPerCycle   = 8;
  const byte fastblinkLoopsOn         = 3;

  // Wombiii timing array
  byte specialTimingArray[3][4] = {
    {1,255,blinkLoopsPerCycle,blinkLoopsOn},
    {1,255,softblinkLoopsPerCycle,softblinkLoopsOn},
    {1,255,fastblinkLoopsPerCycle,fastblinkLoopsOn}
  };

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

  int LightSettings[NumLights][NumStates];                // An array to hold the settings for each state for each light.

  int PriorLightSetting[NumLights][NumStates];            // Sometimes we want to temporarily change the setting for a light. We can store the prior setting here, and revert back to it when the temporary change is over.

  int ActualDimLevel;                                     // We allow the user to enter a Dim level from 0-255. Actually, we do not want them using numbers 0 or 1. The ActualDimLevel corrects for this.
                                                          // In practice, it is unlikely a user would want a dim level of 1 anyway, as it would be probably invisible.
  // STARTUP
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  bool Startup                =  true;                 // This lets us run a few things in the main loop only once instead of over and over

  // DRIVING
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  bool Braking                = false;                 // Are we braking
  bool Decelerating           = false;                 // Are we sharply decelerating
  bool Accelerating           = true;                  // Are we sharply accelerating
  bool StoppedLongTime        = false;                 // Have we been stopped for a long time (actual length of time set on the UserConfig tab - LongStopTime_mS)

  // Backfire effect
  unsigned long backfire_interval;                        // Will save the random interval for the backfire effect
  unsigned long backfire_timeout;                         // Will save the random timeout interval to turn off the LED
  unsigned long Backfire_millis  =  0;                    // will store last time LED was updated
  bool canBackfire            = false;                 // Is the backfiring effect currently active?

  // Overtaking effect
  bool Overtaking             = false;

  // If the user decides to restrict turn signals only to when the car is stopped, they can further add a delay that begins
  // when the car first stops, and until this delay is complete, the turn signals won't come on. This flag indicates if the delay
  // is up. Initialize to true, otherwise turn signals won't work until the car has driven forward once and stopped.
  bool TurnSignal_Enable      =  true;

  // The other situation we want to modify the turn signal is when starting from a stop, while turning. In this case we leave the turn signals on
  // for a period of time even after the car has started moving (typically turn signals are disabled while moving). This mimics a real car where
  // the turn signal isn't cancelled until after the steering wheel turns back the opposite way. In our case we don't check the steering wheel,
  // we just wait a short amount of time (user configurable in AA_UserConfig.h, variable TurnFromStartContinue_mS)
  int TurnSignalOverride         =     0;

  // Blinking effect
  bool Blinker                =  true;                 // A flip/flop variable used for blinking
  bool FastBlinker            =  true;                 // A flip/flop variable used for fast blinking
  bool IndividualLightBlinker[NumLights] = {true, true, true, true, true, true, true, true};   // A flip/flop variable but this time one for each light. Used for SoftBlink.

// Throttle
// Can be used to reverse the throttle channel if they don't have reversing on radio
bool ThrottleChannelReverse;
// A mapped value of ThrottlePulse to (0, MapPulseFwd/Rev) where MapPulseFwd/Rev is the maximum FWD/REV speed (100, or less if governed)
int ThrottleCommand = 0;
// Positive = Forward, Negative = Reverse <ThrottlePulseCenter - ThrottlePulseMin> to <0> to <ThrottlePulseCenter + ThrottlePulseMax>
int ThrottlePulse;
// Will ultimately be determined by setup procedure to read min travel on stick, or from EEPROM if setup complete
int ThrottlePulseMin;
// Will ultimately be determined by setup procedure to read max travel on stick, or from EEPROM if setup complete
int ThrottlePulseMax;
// EX: 1000 + ((2000-1000)/2) = 1500. If Pulse = 1000 then -500, 1500 = 0, 2000 = 500
int ThrottlePulseCenter;
// If small throttle commands do not result in movement due to gearbox/track resistance, increase this number. FOR NOW, LEAVE AT ZERO. IF SET, MUST BE SMALLER THAN THROTTLEDEADBAND
int ThrottleCenterAdjust = 0;

// Steering
// On startup we check to see if this channel is connected, if not, this variable gets set to False and we don't bother checking for it again until reboot
bool SteeringChannelPresent;
// Can be used to reverse the steering channel if they don't have reversing on radio
bool TurnChannelReverse;
// A mapped value of ThrottlePulse from (TurnPulseMin/TurnPulseMax) to MaxLeft/MaxRight turn (100 each way, or less if governed)
int TurnCommand = 0;
// Positive = Right, Negative = Left <TurnPulseCenter - TurnPulseMin> to <0> to <TurnPulseCenter + TurnPulseMax>
int TurnPulse;
// EX: 1000 + ((2000-1000)/2) = 1500. If Pulse = 1000 then -500, 1500 = 0, 2000 = 500
int TurnPulseCenter;
// Will ultimately be determined by setup procedure to read min travel on stick, or from EEPROM if setup complete
int TurnPulseMin;
// Will ultimately be determined by setup procedure to read max travel on stick, or from EEPROM if setup complete
int TurnPulseMax;
// Leave at ZERO for now
int TurnCenterAdjust = 0;

// Channel 3
// On startup we check to see if this channel is connected, if not, this variable gets set to False and we don't bother checking for it again until reboot
bool Channel3Present;
int Channel3Pulse;
int Channel3PulseMin;
int Channel3PulseMax;
int Channel3PulseCenter;

// State of the Channel 3 switch: On (1), Off (0), Disconnected (-1)
int Channel3 = OFF;
bool Channel3Reverse;

// Position defines for Channel 3 switch (can be up to 5 positions)
const byte Pos1 = 0;
const byte Pos2 = 1;
const byte Pos3 = 2;
const byte Pos4 = 3;
const byte Pos5 = 4;


// RC CHANNEL INPUTS
// ------------------------------------------------------------------------------------------------------------------------------------------------>
const byte ThrottleChannel_Pin =     2;                 // The Arduino pin connected to the throttle channel input
const byte SteeringChannel_Pin =    17;                 // The Arduino pin connected to the steering channel input (this is the same as saying pin A3)
const byte Channel3_Pin        =     4;                 // The Arduino pin connected to the Channel 3 input
unsigned long ServoTimeout     = 30000;                 // Value in microseconds (uS) - length of time to wait for a servo pulse. Measured on Eurgle/HK 3channel at ~20-22ms between pulses
                                                        // Up to version 2.03 of OSL code this value was 21,000 (21ms) and it worked fine. However with the release of Arduino IDE 1.6.5,
                                                        // something has changed about the pulseIn function, or perhaps the way it is compiled. At 21ms, pulseIn would return 0 every other read.
                                                        // Increasing the timeout to 30ms seems to have fixed it. LM - 7/15/2015
int PulseMin_Bad               =   700;                 // Pulse widths lower than this amount are considered bad
int PulseMax_Bad               =  2300;                 // Pulse widths greater than this amount are considered bad

// BOARD OBJECTS
// ------------------------------------------------------------------------------------------------------------------------------------------------>
const byte GreenLED            =    18;                 // The Arduino pin connected to the on-board Green LED (this is the same as saying pin A4)
const byte RedLED              =    19;                 // The Arduino pin connected to the on-board Red LED (this is the same as saying pin A5)
const byte SetupButton         =    14;                 // The Arduino pin connected to the on-board push button (this is the same as saying pin A0)

const int LightPin[NumLights] = {9,10,11,6,5,3,15,16};  // These are the Arduino pins to the 8 lights in order from left to right looking down on the top surface of the board.
                                                        // Note that the six Arduino analog pins can be referred to by numbers 14-19
