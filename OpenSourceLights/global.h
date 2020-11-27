#ifndef GLOBAL_H
#define GLOBAL_H

  #include <Arduino.h>
  #include "AA_UserConfig.h"

  extern const byte ThrottleChannel_Pin;
  extern const byte SteeringChannel_Pin;
  extern const byte Channel3_Pin;
  extern const byte Channel4_Pin;
  extern unsigned long ServoTimeout;
  extern int PulseMin_Bad;
  extern int PulseMax_Bad;
  extern const byte GreenLed_Pin;
  extern const byte RedLed_Pin;
  extern const byte SetupButton_Pin;
  extern const byte AuxButton_Pin;
  extern const int LightPin[NumLights];

  extern const byte blinkLoopsPerCycle;
  extern const byte blinkLoopsOn;
  extern const byte softblinkLoopsPerCycle;
  extern const byte softblinkLoopsOn;
  extern const byte fastblinkLoopsPerCycle;
  extern const byte fastblinkLoopsOn;
  extern byte specialTimingArray[3][4];

  extern int MaxFwdSpeed;
  extern int MaxRevSpeed;
  extern int MaxRightTurn;
  extern int MaxLeftTurn;
  extern bool Failsafe;
  extern unsigned long RunCount;
  extern bool CanChangeScheme;
  extern unsigned int BlinkOffID;
  extern bool Blinking;
  extern bool State;
  extern bool PriorState;
  extern int TimesBlinked;
  extern bool ChangeSchemeMode;

  extern const byte Mode1;
  extern const byte Mode2;
  extern const byte Mode3;
  extern const byte Mode4;
  extern const byte Mode5;
  extern const byte StateFwd;
  extern const byte StateRev;
  extern const byte StateStop;
  extern const byte StateStopDelay;
  extern const byte StateBrake;
  extern const byte StateRT;
  extern const byte StateLT;

  extern int LightSettings[NumLights][NumStates];

  extern int PriorLightSetting[NumLights][NumStates];

  extern int ActualDimLevel;
  extern bool Startup;

  extern bool Braking;
  extern bool StoppedLongTime;

  extern bool TurnSignal_Enable;
  extern int TurnSignalOverride;

  extern bool Blinker;
  extern bool FastBlinker;
  extern bool IndividualLightBlinker[NumLights];

    // Variables
  extern int ThrottlePulse;
  extern int ThrottlePulseMin;
  extern int ThrottlePulseMax;
  extern int ThrottlePulseCenter;
  extern int ThrottleCenterAdjust;
  extern int ThrottleCommand;
  extern bool ThrottleChannelReverse;
  extern int TurnPulse;
  extern int TurnPulseMin;
  extern int TurnPulseMax;
  extern int TurnPulseCenter;
  extern int TurnCenterAdjust;
  extern bool SteeringChannelPresent;
  extern bool TurnChannelReverse;
  extern int TurnCommand;
  extern int Channel3Pulse;
  extern int Channel3PulseMin;
  extern int Channel3PulseMax;
  extern int Channel3PulseCenter;
  extern int Channel3;
  extern bool Channel3Present;
  extern bool Channel3Reverse;
  extern int Channel4Pulse;
  extern int Channel4PulseMin;
  extern int Channel4PulseMax;
  extern int Channel4PulseCenter;
  extern int Channel4;
  extern bool Channel4Present;
  extern bool Channel4Reverse;
  extern int CurrentScheme;
  extern const byte Pos1;
  extern const byte Pos2;
  extern const byte Pos3;
  extern const byte Pos4;
  extern const byte Pos5;

#endif // endif GLOBAL_H
