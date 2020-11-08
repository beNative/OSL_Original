/*
 * Open Source Lights   An Arduino-based program to control LED lights on RC vehicles.
 * Version:             3.02
 * Last Updated:        11/07/2019
 * Copyright 2011       Luke Middleton
 *
 * For more information, see the RCGroups thread:
 * http://www.rcgroups.com/forums/showthread.php?t=1539753
 *
 * GitHub Repository:
 * https://github.com/OSRCL/OSL_Original
 *
 *
 * To compile select               Tools -> Board     -> Arduino Duemilanove or Diecimila
 * Also select correct processor   Tools -> Processor -> ATmega328
 *
 *
 * CREDITS!    CREDITS!    CREDITS!
 *----------------------------------------------------------------------------------------------------------------------------------------------------->
 * Several people have contributed code to this project
 *
 * Wombii               RCGroups username Wombii
 *                          October 2019 - light fades are now handled incrementally instaed of sequentially, which means thtat turn signals for exmaple
 *                          no longer occur one after the other but at the same time.
 * Wombii               RCGroups username Wombii
 *                          February 2019 - submitted code to average (smooth) incoming RC commands for those experiencing glitching. It can be enabled for
 *                          any channel on the AA_UserConfig tab. See: https://www.rcgroups.com/forums/showthread.php?1539753-Open-Source-Lights-Arduino-based-RC-Light-Controller/page57#post41222591
 * Richard & Nick       RCGroups username "Rbhoogenboom" and "NickSegers"
 *                          June 2016 - created an Excel file to simplify the light setups. It will automatically generate the entire AA_LIGHT_SETUP file.
 *                          Download from the first post of the thread linked above.
 * Sergio Pizzotti      RCGroups username "wormch"
 *                          March 2015 - Made several impressive changes specifically for drift cars. Wrote all the code related to the backfiring and Xenon effects.
 *                          Made ChangeSchemeMode more user-friendly, it can only be entered after the car has been stopped several seconds.
 *                          Also fixed some bugs and taught me the F() macro!
 * Patrik               RCGroups username "Orque"
 *                          March 2015 - Expanded the Channel 3 functionality to read up to a 5 position switch (previously only worked to 3 positions)
 * Jens                 RCGroups username "learningarduino"
 *                          October 2014 - Fixed bugs related to pin initialization and debug printing.
 * Peter                RCGroups username "4x4_RC_Pit"
 *                          September 2014 - Fixed several bugs in the RadioSetup routine. Also the first person to post a video of OSL in action.
 * JChristensen         We are using Christensen's button library unchanged. It has been renamed from Button to OSL_Button simply because there are many Arduino
 *                      button libraries, and we don't want the install of this one to conflict with others you may already have. See JChristensen's project page here:
 *                      https://github.com/JChristensen/Button
 *
 * Open Source Lights is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v3 as published by
 * the Free Software Foundation (http://www.gnu.org/licenses/)
 *
*/

// ====================================================================================================================================================>
//  INCLUDES
// ====================================================================================================================================================>
#include <JC_Button.h>
#include "global.h"
#include "aa_light_setup.h"
#include "lights.h"
#include "rx_commands.h"
#include "simple_timer.h"
#include "utilities.h"

// Button Object
// Initialize a button objects.
// pin, internal pullup = true, inverted = true, debounce time = 25 mS
Button SetupButton = Button(SetupButton_Pin, 25, true, true);
//Button AuxButton   = Button(AuxButton_Pin, 25, true, true);

// ====================================================================================================================================================>
//  SETUP
// ====================================================================================================================================================>
void setup()
{
  // SERIAL
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  Serial.begin(BaudRate);

  // PINS
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  pinMode(RedLed_Pin, OUTPUT);   // Set RedLED pin to output
  pinMode(GreenLed_Pin, OUTPUT); // Set GreenLED pin to output
  RedLedOff();               // Turn off board LEDs to begin with
  GreenLedOff();

  for (int i = 0; i < NumLights; i++)
  {
    pinMode(LightPin[i], OUTPUT); // Set all the external light pins to outputs
    TurnOffLight(i);              // Start with all lights off
  }

  pinMode(ThrottleChannel_Pin, INPUT_PULLUP); // Set these pins to input, with internal pullup resistors enabled
  pinMode(SteeringChannel_Pin, INPUT_PULLUP);
  pinMode(Channel3_Pin, INPUT_PULLUP);
  pinMode(Channel4_Pin, INPUT_PULLUP);
  pinMode(SetupButton_Pin, INPUT_PULLUP);
  pinMode(AuxButton_Pin, INPUT_PULLUP);

  // CONNECT TO RECEIVER
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  Failsafe = true;                                 // Set failsafe to true
  delay(RX_STARTUP_DELAY);                         // My Flysky receiver seems to need a moment to startup and send PWM before OSL will detect the use of Steering and Channel 3.
  GetRxCommands();                                 // If a throttle signal is measured, Failsafe will turn off
  SteeringChannelPresent = CheckSteeringChannel(); // Check for the presence of a steering channel. If we don't find it here, we won't be checking for it again until the board is rebooted
  Channel3Present = CheckChannel3();               // Check for the presence of Channel 3. If we don't find it here, we won't be checking for it again until the board is rebooted

  // LOAD VALUES FROM EEPROM
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  long Temp;
  // cppcheck-suppress cstyleCast
  eeprom_read(Temp, E_InitNum); // Get our EEPROM Initialization value
  if (Temp != EEPROM_Init)      // If EEPROM has never been initialized before, do so now
  {
    Initialize_EEPROM();
  }
  else
  {
    Load_EEPROM();
  } // Otherwise, load the values from EEPROM

  // RUN LIGHT SETUP
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  SetupLights(CurrentScheme); // Set the lights to the Scheme last saved in EEPROM
  FixDimLevel();              // Takes care of a bug that only occurs if a user sets the Dim level to 1 (unlikely)

  // INITIATE BACKFIRE settings
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Activate the random Seed and set inital random values. These will be set to new random values each time a backfire event occurs, but
  // we need to initialize them for the first event.
  randomSeed(analogRead(0));
  backfire_interval = random(BF_Short, BF_Long);
  backfire_timeout = BF_Time + random(BF_Short, BF_Long);
}

// ====================================================================================================================================================>
//  MAIN LOOP
// ====================================================================================================================================================>
void loop()
{
  // LOCAL VARIABLES
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Drive Modes
  static int DriveModeCommand = UNKNOWN;       // The Drive Mode presently being commanded by user. At startup we don't know, so initialize to UNKNOWN.
  static int DriveMode = STOP;                 // As opposed to DriveModeCommand, this is the actual DriveMode being implemented.
  static int DriveMode_Previous = STOP;        // The previous Drive Mode implemented. There is no "previous" when first started, initialize to STOP
  static int DriveModeCommand_Previous = STOP; // There is no "previous" command when we first start. Initialize to STOP
  static int DriveMode_LastDirection = STOP;
  static int ReverseTaps = 0;

  static int ThrottleCommand_Previous;

  // Shifting Direction
  static int DriveFlag = NO;            // We start with movement allowed
  static unsigned long TransitionStart; // A marker which records the time when the shift transition begins
  static unsigned long StopCMDLength;   // Counter to determine how long we have been commanding Stop

  // Stop time
  static unsigned long TimeStopped; // When did we stop - will be used to initiate a delay after stopping before turn signals can be used, or when stop-delay light settings can take effect

  // Scheme change variables
  static int MaxTurn;                           // This will end up being 90 percent of Turn Command, we consider this a full turn
  static int MinTurn;                           // This will end up being 10-20 percent of Turn Command, this is the minimum movement to be considered a turn command during Change-Scheme-Mode
  static byte RightCount = 0;                   // We use these to count up the number of times the user has cranked the
  static byte LeftCount = 0;                    // steering wheel completely over.
  static boolean ChangeSchemeTimerFlag = false; // Has the timer begun
  static int ChangeModeTime_mS = 2000;          // Amount of time user has to enter Change-Scheme-Mode
  static unsigned int TurnTimerID;              // An ID for the timer that counts the turns of the wheel
  static int ExitSchemeFlag = 0;                // A flag to indicate whether or not to exit Change-Scheme-Mode
  static int TimeToWait_mS = 1200;              // Time to wait between change-scheme commands (otherwise as long as you held the wheel over it would keep cycling through rapidly)
  static int TimeToExit_mS = 3000;              // How long to hold the wheel over until Change-Scheme-Mode is exited
  static unsigned long ExitStart;               // Start time of the exit waiting period
  static boolean TimeoutFlag;
  static boolean HoldFlag;
  static unsigned long HoldStart;

  // Backfire
  static unsigned int BackfireTimerID = 0;
  // Overtaking
  static unsigned int OvertakeTimerID = 0;

  // Temp vars
  static unsigned long currentMillis;
  static boolean WhatState = true;

  // STARTUP - RUN ONCE
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  if (Startup)
  {
    if (DEBUG)
    {
      DumpDebug();
    }                                                      // This puts some useful information to the Serial Port
    timer.setInterval(BlinkInterval, BlinkLights);         // This will call the function BlinkLights every BlinkInterval milliseconds
    timer.setInterval(FastBlinkInterval, FastBlinkLights); // This will call the function FastBlinkLights every FastBlinkInterval milliseconds

    currentMillis = millis(); // Initializing some variables
    TransitionStart = currentMillis;
    StopCMDLength = currentMillis;
    TimeStopped = currentMillis;
    TurnSignal_Enable = false;
    StoppedLongTime = false;

    CanChangeScheme = false;
    ChangeSchemeTimerFlag = false;
    ChangeSchemeMode = false;
    MaxTurn = (int)((float)MaxRightTurn * 0.9); // This sets a turn level that is near max, we use sequential back-and-forth max turns to enter Change-Scheme-Mode
    MinTurn = (int)((float)MaxRightTurn * 0.2); // This is the minimum amount of steering wheel movement considered to be a command during Change-Scheme-Mode
    RightCount = 0;
    LeftCount = 0;

    Startup = false; // This ensures the startup stuff only runs once
  }

  // ETERNAL LOOP
  // ------------------------------------------------------------------------------------------------------------------------------------------------>

  // New light and radio things - Wombii
  // ------------------------------------------------------------------------------------------------------------------------------------------------>

  RunCount++; //loopcounter (Used for the new light switching functions and radio read sequencing

  // UPDATE TIMER/BUTTON STATE
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  timer.run();
  SetupButton.read();

  // USER WANTS TO RUN SETUPS
  // -------------------------------------------------------------------------------------------------------------------------------------------------->
  if (SetupButton.pressedFor(5000))
  {
    // User has held down the input button for two seconds. We are going to enter the radio setup routine.
    RadioSetup();
  }

  // GET COMMANDS FROM RECEIVER
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  GetRxCommands();
  // DETECT IF THE USER WANTS TO ENTER CHANGE-SCHEME-MODE
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // To enter scheme-change-mode, the user needs to turn the steering wheel all the way back and forth at least six times (three each way) in quick succession (within ChangeModeTime_mS milliseconds)
  //if (SteeringChannelPresent && canChangeScheme && (abs(TurnCommand) >= MaxTurn) && !ChangeSchemeMode)
  if (SteeringChannelPresent && CanChangeScheme && !ChangeSchemeMode)
  { // Here we save how many times they have turned the wheel in each direction.
    if ((TurnCommand > 0) && (WhatState == false))
    {
      RightCount += 1;
      WhatState = true;
    }
    else if ((TurnCommand < 0) && (WhatState == true))
    {
      LeftCount += 1;
      WhatState = false;
    }

    if (!ChangeSchemeTimerFlag)
    {
      ChangeSchemeTimerFlag = true;
      TurnTimerID = StartWaiting_mS(ChangeModeTime_mS);
    }

    if (!TimeUp)
    {
      if (((RightCount + LeftCount) > 5) && (RightCount > 2) && (LeftCount > 2))
      {
        ChangeSchemeTimerFlag = false;
        RightCount = 0;
        LeftCount = 0;
        timer.deleteTimer(TurnTimerID);
        ChangeSchemeMode = true; // The user has turned the steering wheel back and forth at least five times - enter Change-Scheme-Mode
      }
    }
  }
  // Time is up, don't enter Change-Scheme-Mode
  if (ChangeSchemeTimerFlag == true && TimeUp)
  {
    ChangeSchemeTimerFlag = false;
    RightCount = 0;
    LeftCount = 0;
    ChangeSchemeMode = false;
  }

  // CHANGE-SCHEME-MODE
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // When Change-Scheme-Mode is first entered, all the lights will blink rapidly for a few seconds. Afterwards, they will blink a sequence of numbers
  // that indicates which Scheme is currently selected.
  // To select a different Scheme, turn the steering wheel briefly to the right (for the next Scheme) or briefly to the left (for the previous Scheme).
  // When you get to the end of the Schemes it will just roll over.
  // Each Scheme number is indicated by the same number of blinks on all lights.
  // When you arrive at the desired Scheme number, hold the steering wheel all the way over to one side (doesn't matter which) and hold it there for
  // three seconds. All lights will blink rapidly once again to indicate exit from Change-Scheme-Mode. The selected Scheme will be saved and the
  // light controller will revert to it even after power is applied.
  // If no control movements are made for 20 seconds, the program will automatically exit Change-Scheme-Mode
  if (ChangeSchemeMode)
  {
    TwinkleLights(2);
    TimesBlinked = 0;
    Blinking = true;
    State = true;
    PriorState = false;
    ExitSchemeFlag = 0;
    TimeoutFlag = false;
    HoldFlag = false;
    do
    {
      // Read the receiver
      GetRxCommands();
      timer.run();

      // They are holding the stick completely over - start waiting to see if they want to
      if (abs(TurnCommand) >= MaxTurn)
      {
        // exit Change-Scheme-Mode
        // If the wait hasn't already begun
        if (ExitSchemeFlag == 0)
        {
          if (TurnCommand > 0)
          {
            ExitSchemeFlag = 1;
          } // Here we remember which direction they are holding the steering wheel
          else if (TurnCommand < 0)
          {
            ExitSchemeFlag = -1;
          }
          ExitStart = millis(); // Start the wait
        }
        // The wait has already begun - check to see if they are still holding full over
        else
        {
          if ((ExitSchemeFlag > 0) && (TurnCommand < MaxTurn))
          {
            ExitSchemeFlag = 0;
          } // They were holding full right but now they're somewhere less
          if ((ExitSchemeFlag < 0) && (TurnCommand > -MaxTurn))
          {
            ExitSchemeFlag = 0;
          } // They were holding full left but now theyr'e somewhere less
        }
        TimeoutFlag = false; // Regardless, we detected movement, so we reset the timeout flag
      }
      else if (abs(TurnCommand) >= MinTurn)
      {
        if (!HoldFlag)
        {
          HoldFlag = true;
          HoldStart = millis();
        }
        ExitSchemeFlag = 0; // If we make it to here they are no longer trying to exit, so reset
        if ((millis() - HoldStart) >= 500)
        {
          HoldFlag = false;
          // Only change scheme if we've waited long enough from last command
          if ((millis() - TransitionStart) >= TimeToWait_mS)
          {
            if (TurnCommand > 0)
            {
              CurrentScheme += 1;
              if (CurrentScheme > NumSchemes)
              {
                CurrentScheme = 1;
              }
            }
            else if (TurnCommand < 0)
            {
              CurrentScheme -= 1;
              if (CurrentScheme < 1)
              {
                CurrentScheme = NumSchemes;
              }
            }
            TransitionStart = millis(); // Force them to wait a bit before changing the scheme again
          }
        }
        TimeoutFlag = false; // Regardless, we detected movement, so we reset the timeout flag
      }
      // In this case, they are not moving the steering wheel at all. We start the timeout timer
      else
      {
        if (!TimeoutFlag)
        {
          TimeoutFlag = true;
          ExitStart = millis();
        }
        ExitSchemeFlag = 0; // But we also reset this flag, because they are obviously not holding the wheel all the way over either
        HoldFlag = false;
      }

      timer.run();
      BlinkAllLights(CurrentScheme);

      if ((ExitSchemeFlag != 0) && ((millis() - ExitStart) >= TimeToExit_mS))
      { // User is exiting out of Change-Scheme-Mode
        ChangeSchemeMode = false;
      }
      else if (TimeoutFlag && ((millis() - ExitStart) >= 20000))
      { // No movement of steering wheel for 20 seconds - automatic exit
        ChangeSchemeMode = false;
      }
    } while (ChangeSchemeMode);
    TwinkleLights(2);
    SetupLights(CurrentScheme); // Change the scheme
    SaveScheme_To_EEPROM();     // Save the selection to EEPROM
  }

  // CALCULATE APPARENT DRIVE MODE
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Figure out what Drive Mode the receiver is indicating
  DriveModeCommand = ReturnDriveMode(ThrottleCommand);

  // CALCULATE ACTUAL DRIVE MODE THAT THE CAR IS PRESENTLY IN
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Drive Mode Command can be one of four things - Forward, Reverse, Stop or Brake. Each one may have some conditions
  // to meet before we let our actual DriveMode = the commanded mode

  // COMMAND STOP
  // -------------------------------------------------------------------------------------------------------------------------------------------->
  // Are we attempting to stop? If so, we only consider ourselves truly stopped if we have been commanding it for a
  // specified length of time. The reason being, the car can take some time to coast to a stop - we may still want to apply brake during
  // the coasting period, and if we set our mode to Stop right away, opposite throttle would not be counted as a brake, but as a change in
  // direction.
  if (DriveModeCommand == STOP)
  {
    // Check to see if we have been commanding stop long enough to change our DriveMode
    if (DriveModeCommand_Previous == STOP)
    {
      // This length of time will be different depending on whether we are stopping from forward (longer) or from reverse (quite short)
      if (DriveMode == FWD)
      {
        if ((millis() - StopCMDLength) >= TimeToStop_FWD_mS)
        {
          DriveMode = STOP; // Throttle has been in the Stop position for TimeToStop_FWD_mS: we assume we are now really stopped.
        }
      }
      else if (DriveMode == REV)
      {
        if ((millis() - StopCMDLength) >= TimeToStop_REV_mS)
        {
          DriveMode = STOP; // Throttle has been in the Stop position for TimeToStop_REV_mS: we assume we are now really stopped.
        }
      }
    }
    //Previously we were commanding something else, so start the stop time counter
    else
    {
      StopCMDLength = millis();
    }
  }
  else
  {
    // If the user is not commanding Stop, reset the TurnSignal_Enable flag.
    TurnSignal_Enable = false;
    // Also reset the canChangeScheme flag, we don't want to enter ChangeScheme mode while moving forward
    CanChangeScheme = false;
    // And finally, reset the StoppedLongTime flag because we are no longer stopped
    StoppedLongTime = false;
  }

  // Turn signal timer - start the timer when the car first comes to a stop.
  // The car has only just now come to a stop. We begin a timer. Not until this timer runs out will a turn command (while still stopped)
  if ((DriveMode == STOP) && (DriveMode_Previous != STOP))
  {
    // actually engage the turn signal lights. This is only if the user has specified BlinkTurnOnlyAtStop = true.
    TurnSignal_Enable = false;
    TimeStopped = millis();
    // We use this same timer to delay the option of entering ChangeScheme mode
    CanChangeScheme = false;
  }
  // We have been stopped already. Check to see if the turn signal signal timer is up - if so, we will permit a turn command (while still stopped)
  else if ((DriveMode == STOP) && (DriveMode_Previous == STOP))
  {
    // to engage the turn signal lights. This is only if the user has specified BlinkTurnOnlyAtStop = true.
    if (TurnSignalDelay_mS > 0 && ((millis() - TimeStopped) >= TurnSignalDelay_mS))
    {
      TurnSignal_Enable = true;
      // After this much time has passed being stopped, we also allow the user to enter ChangeScheme mode if they want
      CanChangeScheme = true;
    }

    // Check to see if we have been stopped a "long" time, this will enable stop-delay effects
    if ((millis() - TimeStopped) >= LongStopTime_mS)
    {
      StoppedLongTime = true;
    }

    // If we are stopped and have been stopped, we are also no longer decelerating, so reset these flags.
    Decelerating = false;
    Accelerating = false;
    CanBackfire  = false;
  }

  // DECELERATING
  // -------------------------------------------------------------------------------------------------------------------------------------------->
  // Here we are trying to identify sharp deceleration commands (user quickly letting off the throttle).
  // The "if" statement says:
  // - if we are not already decelerating, and
  // - if we are presently moving forward, and
  // - if the user is not commanding a reverse throttle, and
  // - if the current throttle command is less than the prior command minus 20 (means, we have let off at least DecelPct steps of throttle since last time - there are 100 steps possible)
  // Then if all this is true, we set the Decelerating flag
  // We are decelerating
  if ((DriveMode == FWD) && (ThrottleCommand >= 0) && (ThrottleCommand < ThrottleCommand_Previous - DecelPct))
  {
    // cppcheck-suppress redundantAssignment
    Decelerating = true;
  }
  // We are not decelerating, clear
  else
  {
    Decelerating = false;
  }
/*
  // BACKFIRE Enable
  // -------------------------------------------------------------------------------------------------------------------------------------------->
  // The last backfire is over, and we have started decelerating again. Enable the backfire effect.
  if (Decelerating && !canBackfire) {
    canBackfire = true;
    // Each backfire event lasts a random length of time.
    BackfireTimerID = timer.setTimeout(backfire_timeout, BackfireOff);
  }
  // disable Backfire effect if the timer has run out
  else if (canBackfire && !timer.isEnabled(BackfireTimerID)) {
    canBackfire = false;
  }
*/  // ACCELERATING
  // -------------------------------------------------------------------------------------------------------------------------------------------->
  // Here we are trying to identify sharp acceleration commands.
  // The "if" statement says:
  // - if we are not already accelerating, and
  // - if we are presently moving forward, and
  // - if the user is not commanding a reverse throttle, and
  // - if the current throttle command is greater than the prior command plus AccelPct (means, we have increased AccelPct throttle since last time)
  // Then if all this is true, we set the Accelerating flag
  // We are accelerating
  if ((DriveMode == FWD) && (ThrottleCommand >= 0) && (ThrottleCommand > ThrottleCommand_Previous + AccelPct))
  {
    Accelerating = true;
  }
  // We are not accelerating, clear
  else
  {
    Accelerating = false;
  }
/*
  // OVERTAKE Enable
  // -------------------------------------------------------------------------------------------------------------------------------------------->
  // Overtaking is simply a timed event that occurs when a heavy acceleration is detected. The length of time the Overtake event lasts is set in
  // UserConfig. During that time, any settings specified under the Acceleration column will take effect. When the timer is up, the lights will
  // revert back to whatever they were previously.
  // Use this with a FASTBLINK setting under the Accelerating column for your headlights, to simulate the overtaking flash as seen in 24hr Le Mans races
  // The last overtaking is over, and we have started accelerating again. Enable the Overtaking timer again.
  if (Accelerating && !Overtaking) {
    Overtaking = true;
    // This will set a timer of OvertakeTime length long, and when the timer expires, it will call the OvertakeOff function
    OvertakeTimerID = timer.setTimeout(OvertakeTime, OvertakeOff);
  }
  // disable Backfire effect if the timer has run out
  else if (Overtaking && !timer.isEnabled(OvertakeTimerID)) {
    Overtaking = false;
  }
*/  // COMMAND BRAKE
  // -------------------------------------------------------------------------------------------------------------------------------------------->
  // If we are braking, turn on the brake light
  Braking = ReturnBrakeFlag(DriveMode_Previous, DriveModeCommand);
  if (Braking)
  {
    DriveModeCommand = STOP; //Braking also counts as a stop command since that is what we will eventually be doing
  }

  // COMMAND FORWARD
  // -------------------------------------------------------------------------------------------------------------------------------------------->
  // Are we attempting to transition to forward, and if so, is it presently allowed?
  // In this case we know we were previously at a stop, and we are now commanding forward
  if (DriveFlag == NO && DriveModeCommand == FWD)
  {
    // We allow the transition if:
    // A) We were moving forward before the stop (commanding same direction as previous), or
    // B) The necessary amount of time has passed for a change in direction
    if ((DriveMode_LastDirection == DriveModeCommand) || ((millis() - TransitionStart) >= TimeToShift_mS))
    {
      DriveMode = FWD;
      DriveFlag = YES;
      ReverseTaps = 0; // Reset the reverse tap count

      // Should we leave the turn signal on even though we're now moving forward?
      // TurnCommand != 0                 The wheels must be turned
      // TurnFromStartContinue_mS > 0     User must have enabled this setting
      // BlinkTurnOnlyAtStop = true       This must be true, otherwise we just allow blinking all the time, so none of this is necessary
      // TurnSignal_Enable = true         This means the car has already been stopped for some length of time, and is not just coasting (set by TurnSignalDelay_mS)
      if (TurnCommand != 0 && TurnFromStartContinue_mS > 0 && BlinkTurnOnlyAtStop && TurnSignal_Enable)
      {
        // In this case we have just begun starting to move, and our wheels are turned at the same time.
        // We will keep the turn signals on for a brief period after starting, as set by TurnFromStartContinue_mS
        TurnSignalOverride = TurnCommand;                                 // TurnSignalOverride saves the turn direction, and will act as a fake turn command in the SetLights function
        timer.setTimeout(TurnFromStartContinue_mS, ClearBlinkerOverride); // ClearBlinkerOverride will set TurnSignalOverride back to 0 when the timer is up.
      }
    }
  }
  else if (DriveModeCommand == FWD && DriveMode_Previous == REV && TimeToShift_mS == 0)
  {
    DriveMode = FWD;
    DriveFlag = YES;
    ReverseTaps = 0;
  }

  // COMMAND REVERSE
  // -------------------------------------------------------------------------------------------------------------------------------------------->
  // Are we attempting to transition to reverse, and if so, is it presently allowed?
  // In this case we know we were previously at a stop, and we are now commanding reverse
  if (DriveFlag == NO && DriveModeCommand == REV)
  {
    // We allow the transition if:
    // A) We were moving in reverse before the stop (commanding same direction as previous)
    if (DriveMode_LastDirection == DriveModeCommand)
    {
      DriveMode = REV;
      DriveFlag = YES;
    }
    // or, we allow it if
    // B) The necessary amount of time has passed for a change in direction AND,
    else if ((millis() - TransitionStart) >= TimeToShift_mS)
    {
      if ((DoubleTapReverse == true && ReverseTaps > 1) || (DoubleTapReverse == false))
      {
        // C) If DoubleTapReverse = true, we also check to make sure the number of reverse taps is > 1
        //    otherwise the time constraint is all we need
        DriveMode = REV;
        DriveFlag = YES;
      }
    }

    // Should we leave the turn signal on even though we're now moving backwards?
    // DriveFlag = YES                  One of the two checks above returned true (A, or B & C) - meaning, we are moving in reverse
    // TurnCommand != 0                 The wheels must be turned
    // TurnFromStartContinue_mS > 0     User must have enabled this setting
    // BlinkTurnOnlyAtStop = true       This must be true, otherwise we just allow blinking all the time, so none of this is necessary
    // TurnSignal_Enable = true         This means the car has already been stopped for some length of time, and is not just coasting (set by TurnSignalDelay_mS)
    if (DriveFlag == YES && TurnCommand != 0 && TurnFromStartContinue_mS > 0 && BlinkTurnOnlyAtStop && TurnSignal_Enable)
    {
      // In this case we have just begun starting to move, and our wheels are turned at the same time.
      // We will keep the turn signals on for a brief period after starting, as set by TurnFromStartContinue_mS
      TurnSignalOverride = TurnCommand;                                 // TurnSignalOverride saves the turn direction, and will act as a fake turn command in the SetLights function
      timer.setTimeout(TurnFromStartContinue_mS, ClearBlinkerOverride); // ClearBlinkerOverride will set TurnSignalOverride back to 0 when the timer is up.
    }
  }
  else if (DriveModeCommand == REV && DriveMode_Previous == FWD && TimeToShift_mS == 0 && DoubleTapReverse == false)
  {
    DriveMode = REV;
    DriveFlag = YES;
    ReverseTaps = 0;
  }

  // WE NOW HAVE OUR ACTUAL DRIVE MODE - SET THE LIGHTS ACCORDINGLY
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  SetLights(DriveMode); // SetLights will take into account whatever position Channel 3 is in, as well as the present drive mode

  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // GET READY FOR NEXT LOOP
  // ------------------------------------------------------------------------------------------------------------------------------------------------>

  // SHIFT TRANSITION TIMER
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // If we have just arrived at a stop, we start the transition timer. This will expire in TimeToShift_mS milliseconds. It is the length of time
  // we must wait before the car is allowed to change direction (either from forwad to reverse or from reverse to forward)
  // This transition time can be set in UserConfig.h. It can even be zero. Some ESCs do require a time however and that is what we are trying to emulate here.

  // Start transition timer when we first reach a stop from a moving direction:
  //Start Timer
  if (DriveMode_Previous != STOP && DriveModeCommand == STOP && DriveFlag == YES)
  {
    TransitionStart = millis();
    // Save last direction
    DriveMode_LastDirection = DriveMode_Previous;
    // While transition is underway, DriveFlag = No
    DriveFlag = NO;
  }

  // COUNT REVERSE TAPS
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Most ESCs require the user to tap reverse twice before the car will actually go into reverse. This is to prevent the user from going straight
  // into reverse from full speed ahead. However, some ESCs allow the user to disable this feature (eg, crawlers), so this setting can also be turned
  // off in UserConfig.h

  // If reverse is commanded, count how many times the stick is moved to reverse - we are only truly moving in reverse on the second tap
  // But we do ignore reverse stick when it is in fact a brake command.
  if (DriveModeCommand == REV && DriveModeCommand_Previous == STOP)
  {
    ReverseTaps += 1;
    if (ReverseTaps > 2)
    {
      ReverseTaps = 2;
    }
  }

  // DEBUGGING
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  if ((DEBUG == true) && (DriveModeCommand_Previous != DriveModeCommand))
  {
    Serial.print(F("Drive Command: "));
    Serial.println(printMode(DriveModeCommand));
  }

  if ((DEBUG == true) && (DriveMode_Previous != DriveMode))
  {
    Serial.print(F("Actual Drive Mode: "));
    Serial.println(printMode(DriveMode));
  }

  //  SAVE COMMANDS FOR NEXT ITERATION
  // ------------------------------------------------------------------------------------------------------------------------------------------------>
  // Set previous variables to current
  DriveMode_Previous        = DriveMode;
  DriveModeCommand_Previous = DriveModeCommand;
  ThrottleCommand_Previous  = ThrottleCommand;
} // End of Loop
