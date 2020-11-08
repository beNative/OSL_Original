#ifndef SIMPLE_TIMER_H
#define SIMPLE_TIMER_H
  #include <Arduino.h>
  #include "OSL_SimpleTimer.h"

  extern OSL_SimpleTimer timer;
  extern bool TimeUp;

  void SetTimeUp();
  void BlinkLights();
  void StopBlinking(unsigned int TimerID);
  void FastBlinkLights();
  unsigned int StartBlinking_ms(int LED, int BlinkTimes, int ms);
  unsigned int StartWaiting_mS(long mS);
  unsigned int StartWaiting_sec(int seconds);

#endif // endef SIMPLE_TIMER_H
