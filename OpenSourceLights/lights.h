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
  void OvertakeOff();
  void ClearBlinkerOverride(void);
  void FastBlinkLight(int WhatLight);
  void FixDimLevel();
  void TwinkleLights(int Seconds);
  void BlinkAllLights(int HowManyTimes);
  void BlinkOn();
  void BlinkOff();
  void BlinkWait();
  void ReverseLight(int WhatLight);

#endif // ndef LIGHTS_H
