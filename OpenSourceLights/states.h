#ifndef STATES_H
#define STATES_H

  #include <Arduino.h>

  // Useful names
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
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

  const byte ON             = 1;
  const byte OFF            = 0;
  const byte YES            = 1;
  const byte NO             = 0;

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
  boolean Startup                =  true;                 // This lets us run a few things in the main loop only once instead of over and over

  // DRIVING
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  boolean Braking                = false;                 // Are we braking
  boolean Decelerating           = false;                 // Are we sharply decelerating
  boolean Accelerating           = true;                  // Are we sharply accelerating
  boolean StoppedLongTime        = false;                 // Have we been stopped for a long time (actual length of time set on the UserConfig tab - LongStopTime_mS)

  // Backfire effect
  unsigned long backfire_interval;                        // Will save the random interval for the backfire effect
  unsigned long backfire_timeout;                         // Will save the random timeout interval to turn off the LED
  unsigned long Backfire_millis  =  0;                    // will store last time LED was updated
  boolean canBackfire            = false;                 // Is the backfiring effect currently active?

  // Overtaking effect
  boolean Overtaking             = false;

  // If the user decides to restrict turn signals only to when the car is stopped, they can further add a delay that begins
  // when the car first stops, and until this delay is complete, the turn signals won't come on. This flag indicates if the delay
  // is up. Initialize to true, otherwise turn signals won't work until the car has driven forward once and stopped.
  boolean TurnSignal_Enable      =  true;

  // The other situation we want to modify the turn signal is when starting from a stop, while turning. In this case we leave the turn signals on
  // for a period of time even after the car has started moving (typically turn signals are disabled while moving). This mimics a real car where
  // the turn signal isn't cancelled until after the steering wheel turns back the opposite way. In our case we don't check the steering wheel,
  // we just wait a short amount of time (user configurable in AA_UserConfig.h, variable TurnFromStartContinue_mS)
  int TurnSignalOverride         =     0;

  // Drive modes
  typedef char DRIVEMODES;
  #define UNKNOWN    0
  #define STOP       1
  #define FWD 	     2
  #define REV        3
  #define LAST_MODE  REV

  // Throttle
  // Can be used to reverse the throttle channel if they don't have reversing on radio
  boolean ThrottleChannelReverse;
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
  int MaxFwdSpeed = 100;
  int MaxRevSpeed = -100;

  // Steering
  // On startup we check to see if this channel is connected, if not, this variable gets set to False and we don't bother checking for it again until reboot
  boolean SteeringChannelPresent;
  // Can be used to reverse the steering channel if they don't have reversing on radio
  boolean TurnChannelReverse;
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
  int MaxRightTurn = 100;
  int MaxLeftTurn = -100;

    // Channel 3
  // On startup we check to see if this channel is connected, if not, this variable gets set to False and we don't bother checking for it again until reboot
  boolean Channel3Present;
  int Channel3Pulse;
  int Channel3PulseMin;
  int Channel3PulseMax;
  int Channel3PulseCenter;
  // State of the Channel 3 switch: On (1), Off (0), Disconnected (-1)
  int Channel3 = OFF;
  boolean Channel3Reverse;
  // Position defines for Channel 3 switch (can be up to 5 positions)
  const byte Pos1 = 0;
  const byte Pos2 = 1;
  const byte Pos3 = 2;
  const byte Pos4 = 3;
  const byte Pos5 = 4;

#endif // ndef STATES_H
