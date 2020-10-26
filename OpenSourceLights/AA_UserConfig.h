#ifndef AA_USERCONFIG_H
#define AA_USERCONFIG_H
#include <Arduino.h>
// THIS FILE HAS SETTINGS WHICH THE USER CAN CONFIGURE

// NUMBER OF SCHEMES
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // The number of lighting schemes implemented. Theoretically it can be anything up the memory limit. Defaults to 2.
  // MAKE SURE THIS NUMBER MATCHES THE NUMBER OF SCHEMES DEFINED IN AA_LIGHT_SETUP !!
  #define NumSchemes 6

  // The number of light outputs available on the board
  #define NumLights 8

// STATE SETTINGS
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // The Stop Delay state only occurs when the vehicle has been stopped for some length of time, which is set here.
  // Recall that 1000 mS = 1 second (default value is 30 seconds)
  #define LongStopTime_mS 3000

  // There are 14 possible states a light can be in:
  // - Mode 1, Mode 2, Mode 3, Mode 4, Mode 5 (all from Channel3 switch),
  // - Forward, Reverse, Stop, Stop Delay, Brake (from Throttle Channel),
  // - Right Turn, Left Turn (from Turn Channel)
  // - Accelerating -
  // - Decelerating - special state that occurs on heavy deceleration (from Throttle Channel)
  #define NumStates 14

// LIGHT SETTINGS - DIM LEVEL
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Number from 0-255, with 0 being off, 255 being full on. Often numbers much greater than half (128) are hard to distinguish from full on.
  // Experiment to get the number that makes your lights as dim as you want them.
  // - Mode 1, Mode 2, Mode 3, Mode 4, Mode 5 (all from Channel3 switch),
  // - Forward, Reverse, Stop, Stop Delay, Brake (from Throttle Channel),
  // - Right Turn, Left Turn (from Turn Channel)
  // - Accelerating -
  // - Decelerating - special state that occurs on heavy deceleration (from Throttle Channel)
  #define DimLevel                    25

// LIGHT SETTINGS - BLINKING
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // A value in milliseconds that sets the blink rate for blinking lights set to "BLINK" or "SOFTBLINK" (for example, turn signals). 1000 = 1 second
  #define BlinkInterval              750
  // A value in milliseconds that sets the fast blink rate for lights set to "FASTBLINK"
  #define FastBlinkInterval           15
  // The SOFTBLINK effect fades the light in and out as it blinks. Each fade consists of 20 steps. You can set the amount of delay between each
  #define SoftBlinkFadeInDelay         6
  // step, and the delay can be different for fade in and out. Take for example a FadeInDelay of 5 mS. There are 20 steps to fade in,
  // so 20 * 5 = 100mS for the light to fade in. The total amount of time for the fade in + the fade out should not exceed BlinkInterval.
  #define SoftBlinkFadeOutDelay       17

// LIGHT SETTINGS - TURN SIGNALS
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // If you only want your turn signal blinkers to come on when the car is Stopped, set this to true.
  // Turn signals are cool, but they look silly when they start blinking every time you turn the steering wheel while driving.
  // For that reason, you will probably want to keep this "true"
  // NOTE: This only applies to a BLINK or SOFTBLINK setting in the "RightTurn" or "LeftTurn" states.
  // Any setting other than BLINK or SOFTBLINK in the "RightTurn" or "LeftTurn" column will NOT be affected.
  #define BlinkTurnOnlyAtStop       true
  // Set to true to restrict all other turn settings (not just BLINK and SOFTBLINK) to the same conditions imposed by BlinkTurnOnlyAtStop
  #define AllTurnSettingsMatch      false

  // If BlinkTurnOnlyAtStop = true, this setting further refines when the turn signals can come on. Instead of coming on right when the
  // car reaches a stop, you can set a delay in milliseconds (1000 mS = 1 second) before they will be enabled. This way, if you come
  // to a stop while the wheels are turned, the turn signals will not come on instantly, which looks very strange.
  // Instead there will be a delay of TurnSignalDelay_mS milliseconds after which you can hold the wheels over and the turn signals will come on.
  // Once again we are trying to prevent the unrealistic engagement of turn signals, but rather have them only engaged when you specifically
  // want to for display purposes.
  #define TurnSignalDelay_mS        1500
  // If BlinkTurnOnlyAtStop = true, this setting determines the length of time the turn signal will continue to blink when you begin moving from
  // a stop with the wheels turned. In a real car, the blinker remains on through the turn but then is cancelled after the steering wheel returns
  // to center. That is the effect we are trying to mimic, but we don't do it by checking the steering wheel, we simply set a length of time for the
  // turn signal to continue blinking. If you don't want this effect to happen, set this to 0 (zero).
  #define TurnFromStartContinue_mS  3000

// DOUBLE TAP REVERSE
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Most ESCs require you to tap reverse twice before the car actually goes into reverse.
  // If yours is like this, set it to true. Most touring cars operate like this.
  // But if you can shift directly into reverse from forward, set this to false - this is typical of crawlers.
  #define DoubleTapReverse          false

// ACCELERATING AND DECELERATING
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // How much does the throttle have to increase (1-100 pct) to be considered a sharp acceleration.
  // This will trigger the OvertakeTime timer set below, during which your lights will do whatever
  // setting you put in the Accelerating column (BLINK or FASTBLINK makes sense, like they do in
  // 24hr Le Mans when overtaking)
  #define AccelPct                    35
  // How long should the overtake event last in  ms (1000ms = 1 second)
  #define OvertakeTime               500

  // How much does the throttle need to decrease (1-100 pct) to be considered a sharp deceleration.
  // This will trigger the Backfire effect for any light in the Decelerating column with the setting
  // of BACKFIRE. You can put other settings in the Decelerating column besides Backfire, and they will
  // work, but they will only be enabled for the same length of time as the backfire event
  #define DecelPct                    20
  // How long in milliseconds (1000 ms = 1 second) on average should a backfire event last. It will actually be
  // a random length of time spanning from (BF_Time - BF_Long) to (BF_Time + BF_Long)
  #define BF_Time                    170
  // BF_Short and BF_Long are the upper and lower limits to the span of time the backfiring LED will blink.
  #define BF_Short                    10
  // In other words, while backfiring the LED will blink randomly on and off for some value between BF_Short and BF_Long
  #define BF_Long                     60

// COASTING
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // If we didn't allow any time for the car to coast to a stop, there would be no need for braking, and your brake lights would never come on. However in real life,
  // your car does coast even after you let off the 'gas'. During this time, opposite throttle commands are actually counted as a command to change direction, but instead
  // are counted as braking. Tweak the coast times here to match what you see in real life. They are in milliseconds.
  // 1000 ms = 1 second
  #define TimeToStop_FWD_mS          500          // An estimate of the time usually spent coasting to a stop from forward. During this time, reverse commands will be counted as braking
  #define TimeToStop_REV_mS          500          // An estimate of the time usually spent coasting to a stop from reverse. During this time, forward commands will be counted as braking

  // If DragBrake = false, the Brake state will be active only when your car is moving one direction, and you command an opposite direction
  // If DragBrake = true, the Brake state will still occur in the above example, but it will also occur anytime your throttle stick is near center.
  // In other words, setting this to true essentially has your brakes come on automatically whenever you are coasting. For touring cars, this
  // can sometimes make for a nice effect going round a track (brake lights should come on in advance of a curve, for example).
  // NOTE: If you want your brakes to come on automatically whenever you are STOPPED, you should set that in the LIGHT_SETUP tab under the Stop column.
  #define DragBrake                 false

// SHIFTING TIME
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // How much of a pause is required before changing directions (from forward to reverse or from reverse to forward).
  // For most ESCs this will be close to zero.
  #define TimeToShift_mS             100          // The pause time in milliseconds that will be required before the vehicle is allowed to change direction

// DEADBAND
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // This reduces the sensitivity around center stick. The numbers can be 0-100 but should be rather small, like 10. This prevents minor movements of your sticks when
  // stopped from  setting off your lights.
  // Note: if you find you need to set these numbers to high values, what you probably need is to run through Radio Setup instead.
  #define ThrottleDeadband 10     // Throttle channel hysteriesis. Values below this will be ignored. Default is 10, number should be small.
  #define TurnDeadband     70     // Same thing, but for steering channel.


// RC Input Smoothing
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Enable if you experience glitching on any channel. Most users do not need it.
  #define SmoothThrottle            false
  #define SmoothSteering            false
  #define SmoothChannel3            false
  #define smoothingStrength         3             // Number from 0-4. Change this value to set strength of smoothing. Use minimum acceptable value.

// DEBUGGING
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Set this to true to receive debugging messages out the serial port. NOTE: This will make the program less responsive,
  // so turn it off when you are done troubleshooting.
  #define DEBUG false

  // If this is set to true, the Green LED on the board will be on whenever the car is moving forward,
  // the Red LED will come on whenever the car is moving in reverse,
  // both LEDs will turn OFF when the car is stopped,
  // both LEDs will turn ON when the car is braking,
  // the Red LED will blink quickly if you are turning left, and
  // the Green LED will blink quickly if you are turning right.
  // You can use these to verify the board is working correctly without having any lights connected.
  // LED_DEBUG does not affect the performance of the circuit, so you can leave it on.
  #define LED_DEBUG true

  // If true, all eight LED outputs will blink rapidly when the radio signal has been lost.
  // If set to false, only the onboard Red and Green LEDs will blink when the radio signal has been lost
  #define BLINK_LIGHTS_RX_LOST true

  // I found I needed to delay the first read that checks if the Steering and Channel 3 is in use, in order to get reliable startup using those channels.
  #define RX_STARTUP_DELAY 200

// SERIAL
// ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Set baud rate here if you know what you're doing and don't like the default value
  #define BaudRate                 38400

#endif // endef AA_USERCONFIG_H
