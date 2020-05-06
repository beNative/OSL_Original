#ifndef DRIVE_H
#define DRIVE_H
  #include <Arduino.h>

  extern const byte ThrottleDeadband;
  extern const byte TurnDeadband;
  extern int ThrottlePulseMin;
  extern int ThrottlePulseMax;
  extern int ThrottlePulseCenter;
  extern int TurnPulseMin;
  extern int TurnPulseMax;
  extern int TurnPulseCenter;
  extern int Channel3PulseMin;
  extern int Channel3PulseMax;
  extern int Channel3PulseCenter;
  extern boolean ThrottleChannelReverse;
  extern boolean TurnChannelReverse;
  extern boolean Channel3Reverse;
  extern int CurrentScheme;

  // Drive modes
  typedef char DRIVEMODES;
  #define UNKNOWN    0
  #define STOP       1
  #define FWD 	     2
  #define REV        3
  #define LAST_MODE  REV

  int ReturnDriveMode(int ThrottleCMD);
  boolean ReturnBrakeFlag(int DriveModePrev, int DriveModeCMD);
#endif // ndef DRIVE_H
