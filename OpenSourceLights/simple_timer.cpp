#include <Arduino.h>
#include "simple_timer.h"
#include "global.h"
#include "lights.h"

OSL_SimpleTimer timer;
bool TimeUp = true;

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// BLINKLIGHTS - This flip/flops our blinker variable
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void BlinkLights()
{
  Blinker = !Blinker;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// FASTBLINKLIGHTS - This flip/flops our fast blinker variable
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void FastBlinkLights()
{
  FastBlinker = !FastBlinker;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// STARTBLINKING_MS - Used by the Radio_Setup routines, this will blink a given LED a certain number of times and then stop
// ------------------------------------------------------------------------------------------------------------------------------------------------>
unsigned int StartBlinking_ms(int LED, int BlinkTimes, int ms)
{
  switch (BlinkTimes)
  {
  case 1:
    if (LED == GreenLed_Pin)
    {
      return timer.setInterval(ms, GreenBlinkOne);
    }
    if (LED == RedLed_Pin)
    {
      return timer.setInterval(ms, RedBlinkOne);
    }
    break;
  case 2:
    if (LED == GreenLed_Pin)
    {
      return timer.setInterval(ms, GreenBlinkTwo);
    }
    if (LED == RedLed_Pin)
    {
      return timer.setInterval(ms, RedBlinkTwo);
    }
    break;
  case 3:
    if (LED == GreenLed_Pin)
    {
      return timer.setInterval(ms, GreenBlinkThree);
    }
    if (LED == RedLed_Pin)
    {
      return timer.setInterval(ms, RedBlinkThree);
    }
    break;
  default:
    break;
  }
}

void StopBlinking(unsigned int TimerID)
{
  timer.deleteTimer(TimerID);
}

unsigned int StartWaiting_mS(long mS)
{
  TimeUp = false;
  return timer.setTimeout(mS, SetTimeUp); // will call function once after ms duration
}

unsigned int StartWaiting_sec(int seconds)
{
  return StartWaiting_mS(seconds * 1000);
}

void SetTimeUp()
{
  TimeUp = true;
}
