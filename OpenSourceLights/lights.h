#ifndef LIGHTS_H
#define LIGHTS_H

  #include <Arduino.h>
  #include "AA_UserConfig.h"

  extern int CurrentScheme;
  extern int PWM_Step[NumLights];
  extern int Xenon_EffectDone[NumLights];

  void SetupLights(int WhatScheme);
  void SetLights(int DriveMode);
  void DumpLightSchemeToSerial(int WhatScheme);
  void SetLight(int WhatLight, int WhatSetting);
  byte SimpleFader(byte currentPin, byte wantedFadeSetting, byte wantedOutput);
  byte flashingFunctionStartActiveSimple ( byte functionNumber, byte tempArray[4]);
  void TurnOnLight(int WhatLight);
  void TurnOffLight(int WhatLight);
  void LightBackfire(int WhatLight);
  void BackfireOff();
  void OvertakeOff();  void ClearBlinkerOverride(void);
  void FastBlinkLight(int WhatLight);
  void FixDimLevel();
  void TwinkleLights(int Seconds);
  void BlinkAllLights(int HowManyTimes);
  void BlinkOn();
  void BlinkOff();
  void BlinkWait();
  void ReverseLight(int WhatLight);
  // Functions for Onboard LED's
  void RedLedOn();
  void RedLedOff();
  void GreenLedOn();
  void GreenLedOff();
  void GreenBlink();
  void RedBlink();
  void ToggleAllLights();
  void GreenBlinkSlow(int HowMany);
  void RedBlinkFast(int HowMany);
  void GreenBlinkFast(int HowMany);
  // If these seem redundant it is because SimpleTimer can only call a void function
  void GreenBlinkOne();
  void GreenBlinkTwo();
  void GreenBlinkThree();
  void RedBlinkOne();
  void RedBlinkTwo();
  void RedBlinkThree();

#endif // ndef LIGHTS_H
