#ifndef DRIVE_H
#define DRIVE_H
  #include <Arduino.h>

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
