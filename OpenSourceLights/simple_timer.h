#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H
  #include <Arduino.h>
  #include "OSL_SimpleTimer.h"

  extern OSL_SimpleTimer timer;
  extern bool TimeUp;

  void BlinkLights();
  void FastBlinkLights();
  unsigned int StartBlinking_ms(int LED, int BlinkTimes, int ms);
  void StopBlinking(unsigned int TimerID);
  unsigned int StartWaiting_mS(int mS);
  unsigned int StartWaiting_sec(int seconds);
  void SetTimeUp();

#endif // ndef SIMPLE_TIMER_H
