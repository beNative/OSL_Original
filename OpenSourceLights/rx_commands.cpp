#include <Arduino.h>
#include "AA_UserConfig.h"
#include "global.h"
#include "lights.h"
#include "rx_commands.h"
#include "simple_timer.h"
#include "utilities.h"

// This update makes sure every loop is as fast as possible because we only read one channel per loop - Wombii
// Loop speed gain is up to 2-3x
void GetRxCommands()
{
  static uint8_t failsafeCounter = 0;

  if (Failsafe)
  {
    failsafeCounter++;
    while (failsafeCounter > 4) // Don't go into failsafe until we've missed 4 sequential packets
    {
      if (DEBUG)
        Serial.println(F("RX Disconnected!"));
      ToggleAllLights(); // If the receiver isn't connected, pause the program and flash all the lights
      delay(50);
      GetThrottleCommand(); // If we can successfully read the throttle channel, we will go out of failsafe
      if (Failsafe == false)
        failsafeCounter = 0; // We're out of failsafe, exit this while
    }
  }
  else
  {
    failsafeCounter = 0;
  }

  byte channelSelector;
  channelSelector = RunCount % 4;
  switch (channelSelector)
  {
  case 0:
    ThrottleCommand = GetThrottleCommand();
    break;
  case 1:
    if (SteeringChannelPresent)
    {
      TurnCommand = GetTurnCommand();
    }
    else
    {
      TurnCommand = 0;
    } // We set Turn to nothing if not being used
    break;
  case 2:
    ThrottleCommand = GetThrottleCommand();
    break;
  case 3:
    if (Channel3Present)
    {
      Channel3 = GetChannel3Command();
    }
    else
    {
      Channel3 = Pos1;
    } // We set Channel 3 to Position 1 if not being used
    break;
  }
}

boolean CheckChannel3()
{
  Channel3Pulse = pulseIn(Channel3_Pin, HIGH, ServoTimeout * 2);
  if (Channel3Pulse == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

boolean CheckChannel4()
{
  Channel4Pulse = pulseIn(Channel4_Pin, HIGH, ServoTimeout * 2);
  if (Channel4Pulse == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

boolean CheckSteeringChannel()
{
  TurnPulse = pulseIn(SteeringChannel_Pin, HIGH, ServoTimeout * 2);
  if (TurnPulse == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}

int GetThrottleCommand()
{
  int ThrottleCommand;
  ThrottlePulse = pulseIn(ThrottleChannel_Pin, HIGH, ServoTimeout);

  if ((ThrottlePulse == 0) || (ThrottlePulse > PulseMax_Bad) || (ThrottlePulse < PulseMin_Bad))
  { // Timed out waiting for a signal, or measured a bad signal
    // Set Failsafe flag, set Throttle to 0
    Failsafe = true;
    ThrottleCommand = 0;
    return constrain(ThrottleCommand, MaxRevSpeed, MaxFwdSpeed);
  }
  else
  {
    Failsafe = false;
    if (ThrottlePulse >= ThrottlePulseCenter + ThrottleDeadband)
    {
      if (ThrottleChannelReverse == false)
      {
        // Without channel reversing, we consider PPM pulse values greater than Center to be forward throttle commands. We now map the radio command to our forward throttle range
        ThrottleCommand = map(ThrottlePulse, (ThrottlePulseCenter + ThrottleDeadband), ThrottlePulseMax, ThrottleCenterAdjust, MaxFwdSpeed);
      }
      else
      {
        // With channel reversing, we consider PPM pulse values greater than Center to be reverse throttle commands. We now map the radio command to our reverse throttle range
        ThrottleCommand = map(ThrottlePulse, (ThrottlePulseCenter + ThrottleDeadband), ThrottlePulseMax, -ThrottleCenterAdjust, MaxRevSpeed);
      }
    }
    else if (ThrottlePulse <= (ThrottlePulseCenter - ThrottleDeadband))
    {
      if (ThrottleChannelReverse == false)
      {
        // Without channel reversing, we consider PPM pulse values less than Center to be reverse throttle commands. We now map the radio command to our reverse throttle range
        ThrottleCommand = map(ThrottlePulse, ThrottlePulseMin, (ThrottlePulseCenter - ThrottleDeadband), MaxRevSpeed, ThrottleCenterAdjust);
      }
      else
      {
        // With channel reversing, we consider PPM pulse values less than Center to be forward throttle commands. We now map the radio command to our forward throttle range
        ThrottleCommand = map(ThrottlePulse, ThrottlePulseMin, (ThrottlePulseCenter - ThrottleDeadband), MaxFwdSpeed, -ThrottleCenterAdjust);
      }
    }
    else
    { // In this case we are within the ThrottleDeadband setting, so Command is actually Zero (0)
      ThrottleCommand = 0;
    }

    // Average the command if user has this option enabled
    if (SmoothThrottle)
      ThrottleCommand = smoothThrottleCommand(ThrottleCommand);

    return constrain(ThrottleCommand, MaxRevSpeed, MaxFwdSpeed);
    // After all this, ThrottleCommand is now some value from -255 to +255 where negative equals REV and positive equals FWD. (The values can actually be less if top forward or
    // reverse speeds have been limited by user)
  }
}

int GetTurnCommand()
{
  int TurnCommand;
  TurnPulse = pulseIn(SteeringChannel_Pin, HIGH, ServoTimeout);
  // In this case, there was no signal found on the turn channel
  if ((TurnPulse == 0) || (TurnPulse > PulseMax_Bad) || (TurnPulse < PulseMin_Bad))
  {
    TurnCommand = 0; // If no TurnPulse, we set Turn to 0 (no turn)
    return constrain(TurnCommand, MaxLeftTurn, MaxRightTurn);
  }
  else
  {
    if (TurnPulse >= TurnPulseCenter + TurnDeadband)
    {
      if (TurnChannelReverse == false)
      {
        // Without channel reversing, we consider PPM pulse values greater than Center to be Right Turn commands. We now map the radio command to our Right Turn range
        TurnCommand = map(TurnPulse, (TurnPulseCenter + TurnDeadband), TurnPulseMax, TurnCenterAdjust, MaxRightTurn);
      }
      else
      {
        // With channel reversing, we consider PPM pulse values greater than Center to be Left Turn commands. We now map the radio command to our Left Turn range
        TurnCommand = map(TurnPulse, (TurnPulseCenter + TurnDeadband), TurnPulseMax, -TurnCenterAdjust, MaxLeftTurn);
      }
    }
    else if (TurnPulse <= (TurnPulseCenter - TurnDeadband))
    {
      if (TurnChannelReverse == false)
      {
        // Without channel reversing, we consider PPM pulse values less than Center to be Left Turn commands. We now map the radio command to our Left Turn range
        TurnCommand = map(TurnPulse, TurnPulseMin, (TurnPulseCenter - TurnDeadband), MaxLeftTurn, -TurnCenterAdjust);
      }
      else
      {
        // With channel reversing, we consider PPM pulse values less than Center to be Right Turn commands. We now map the radio command to our Right Turn range
        TurnCommand = map(TurnPulse, TurnPulseMin, (TurnPulseCenter - TurnDeadband), MaxRightTurn, TurnCenterAdjust);
      }
    }
    else
    {
      // In this case we are within the TurnDeadband setting, so Command is actually Zero (0)
      TurnCommand = 0;
    }

    // Average the command if user has this option enabled
    if (SmoothSteering)
      TurnCommand = smoothSteeringCommand(TurnCommand);

    return constrain(TurnCommand, MaxLeftTurn, MaxRightTurn);
    // After all this, TurnCommand is now some value from -100 to +100 where negative equals LEFT and positive equals RIGHT.
  }
}

int GetChannel3Command()
{
  int Channel3Command;
  Channel3Pulse = pulseIn(Channel3_Pin, HIGH, ServoTimeout);

  if (Channel3Pulse == 0)
  { // In this case, there was no signal found
    Channel3Command = Pos1; // If no Channel3, we always set the mode to 1
  }
  else
  {
    Channel3Present = true;

    // Turn pulse into one of five possible positions
    if (Channel3Pulse >= Channel3PulseMax - 150)
    {
      Channel3Command = Pos5;
    }
    else if ((Channel3Pulse > (Channel3PulseCenter + 100)) && (Channel3Pulse < (Channel3PulseMax - 150)))
    {
      Channel3Command = Pos4;
    }
    else if ((Channel3Pulse >= (Channel3PulseCenter - 100)) && (Channel3Pulse <= (Channel3PulseCenter + 100)))
    {
      Channel3Command = Pos3;
    }
    else if ((Channel3Pulse < (Channel3PulseCenter - 100)) && (Channel3Pulse > (Channel3PulseMin + 150)))
    {
      Channel3Command = Pos2;
    }
    else
    {
      Channel3Command = Pos1;
    }

    // Swap positions if Channel 3 is reversed.
    if (Channel3Reverse)
    {
      if (Channel3Command == Pos1)
        Channel3Command = Pos5;
      else if (Channel3Command == Pos2)
        Channel3Command = Pos4;
      else if (Channel3Command == Pos4)
        Channel3Command = Pos2;
      else if (Channel3Command == Pos5)
        Channel3Command = Pos1;
    }
  }

  // Average the command if user has this option enabled
  if (SmoothChannel3)
    Channel3Command = smoothChannel3Command(Channel3Command);

  return Channel3Command;
}

int GetChannel4Command()
{
  int Channel4Command;
  Channel4Pulse = pulseIn(Channel4_Pin, HIGH, ServoTimeout);

  if (Channel4Pulse == 0)
  { // In this case, there was no signal found
    Channel4Command = Pos1; // If no Channel3, we always set the mode to 1
  }
  else
  {
    Channel4Present = true;

    // Turn pulse into one of five possible positions
    if (Channel4Pulse >= Channel4PulseMax - 150)
    {
      Channel4Command = Pos5;
    }
    else if ((Channel4Pulse > (Channel4PulseCenter + 100)) && (Channel4Pulse < (Channel4PulseMax - 150)))
    {
      Channel4Command = Pos4;
    }
    else if ((Channel4Pulse >= (Channel4PulseCenter - 100)) && (Channel4Pulse <= (Channel4PulseCenter + 100)))
    {
      Channel4Command = Pos3;
    }
    else if ((Channel4Pulse < (Channel4PulseCenter - 100)) && (Channel4Pulse > (Channel4PulseMin + 150)))
    {
      Channel4Command = Pos2;
    }
    else
    {
      Channel4Command = Pos1;
    }

    // Swap positions if Channel 3 is reversed.
    if (Channel4Reverse)
    {
      if (Channel4Command == Pos1)
        Channel4Command = Pos5;
      else if (Channel4Command == Pos2)
        Channel4Command = Pos4;
      else if (Channel4Command == Pos4)
        Channel4Command = Pos2;
      else if (Channel4Command == Pos5)
        Channel4Command = Pos1;
    }
  }

  return Channel4Command;
}

void RadioSetup()
{
  int Count;
  unsigned long TotThrottlePulse = 0;
  unsigned long TotTurnPulse     = 0;
  unsigned long TotChannel3Pulse = 0;
  float TempFloat;

  int TypicalPulseCenter = 1500;
  int MaxPossiblePulse   = 2250;
  int MinPossiblePulse   = 750;

  int GreenBlinker; // We'll use this to create a SimpleTimer for the green led, that flashes to represent which stage we are in.

  // RUN SETUP
  // -------------------------------------------------------------------------------------------------------------------------------------------------->
  Serial.println(F("ENTERING SETUP..."));
  Serial.println();

  // While in setup, Red LED remains on:
  RedLedOn();

  // STAGE 1 = Read max travel values from radio, save to EEPROM
  // ------------------------------------------------------------------------------------------------------------------------------------------>
  // Transition to Stage 1:
  // Green LED on steady for two seconds
  delay(2000);
  Serial.println(F("STAGE 1 - STORE MAX TRAVEL VALUES"));
  PrintHorizontalLine();
  Serial.println(F("Move all controls to maximum values"));
  Serial.println(F("while green LED blinks"));
  Serial.println();
  GreenLedOn();
  delay(2000);
  GreenLedOff();
  delay(2000);
  TimeUp = false;
  // Start green LED blinking for stage one: one blink every 1200 ms
  GreenBlinker = StartBlinking_ms(GreenLed_Pin, 1, 1200); // Blip the GreenLED once every 1200ms
  StartWaiting_sec(15); // default 15
  Serial.println(F("Reading..."));
  Serial.println(F("Started reading"));

  // We initialize every min and max value to TypicalPulseCenter. In the loop below we will record deviations from the center.
  ThrottlePulseMin = TypicalPulseCenter;
  ThrottlePulseMax = TypicalPulseCenter;
  TurnPulseMin     = TypicalPulseCenter;
  TurnPulseMax     = TypicalPulseCenter;
  Channel3PulseMin = TypicalPulseCenter;
  Channel3PulseMax = TypicalPulseCenter;

  int i = 0;// Repeat until StartWaiting timer is up and at least 100 cycles are elapsed
  do
  {
    // Read channel while the user moves the sticks to the extremes
    ThrottlePulse = pulseIn(ThrottleChannel_Pin, HIGH, ServoTimeout);
    TurnPulse     = pulseIn(SteeringChannel_Pin, HIGH, ServoTimeout);
    Channel3Pulse = pulseIn(Channel3_Pin, HIGH, ServoTimeout);

    // Each time through the loop, only save the extreme values if they are greater than the last time through the loop.
    // At the end we should have the true min and max for each channel.
    if (ThrottlePulse > ThrottlePulseMax)
    {
      ThrottlePulseMax = ThrottlePulse;
    }
    if (TurnPulse > TurnPulseMax)
    {
      TurnPulseMax = TurnPulse;
    }
    if (Channel3Pulse > Channel3PulseMax)
    {
      Channel3PulseMax = Channel3Pulse;
    }
    // However we don't save a min pulse if it is equal to zero, because that is not valid.
    if (ThrottlePulse > 0 && ThrottlePulse < ThrottlePulseMin)
    {
      ThrottlePulseMin = ThrottlePulse;
    }
    if (TurnPulse > 0 && TurnPulse < TurnPulseMin)
    {
      TurnPulseMin = TurnPulse;
    }
    if (Channel3Pulse > 0 && Channel3Pulse < Channel3PulseMin)
    {
      Channel3PulseMin = Channel3Pulse;
    }
    delay(100);
    // Refresh the timer
    timer.run();
    Serial.print(".");
    i++;
  } while (!TimeUp || (i < 50)); // Keep looping until time's up

  Serial.println(F("Stopped reading"));

  StopBlinking(GreenBlinker);

  // Sanity check in case something weird happened (like Tx turned off during setup, or some channels disconnected)
  if (ThrottlePulseMin < MinPossiblePulse)
  {
    ThrottlePulseMin = MinPossiblePulse;
  }
  if (TurnPulseMin < MinPossiblePulse)
  {
    TurnPulseMin = MinPossiblePulse;
  }
  if (Channel3PulseMin < MinPossiblePulse)
  {
    Channel3PulseMin = MinPossiblePulse;
  }
  if (ThrottlePulseMax > MaxPossiblePulse)
  {
    ThrottlePulseMax = MaxPossiblePulse;
  }
  if (TurnPulseMax > MaxPossiblePulse)
  {
    TurnPulseMax = MaxPossiblePulse;
  }
  if (Channel3PulseMax > MaxPossiblePulse)
  {
    Channel3PulseMax = MaxPossiblePulse;
  }

  // Save values to EEPROM
  eeprom_write(ThrottlePulseMin, E_ThrottlePulseMin);
  eeprom_write(ThrottlePulseMax, E_ThrottlePulseMax);
  eeprom_write(TurnPulseMin, E_TurnPulseMin);
  eeprom_write(TurnPulseMax, E_TurnPulseMax);
  eeprom_write(Channel3PulseMin, E_Channel3PulseMin);
  eeprom_write(Channel3PulseMax, E_Channel3PulseMax);

  Serial.println();
  Serial.println(F("Stage 1 Results: Min - Max pulse values"));
  Serial.print(F("Throttle: "));
  Serial.print(ThrottlePulseMin);
  PrintSpaceDash();
  Serial.println(ThrottlePulseMax);
  Serial.print(F("Turn: "));
  Serial.print(TurnPulseMin);
  PrintSpaceDash();
  Serial.println(TurnPulseMax);
  Serial.print(F("Ch3: "));
  Serial.print(Channel3PulseMin);
  PrintSpaceDash();
  Serial.println(Channel3PulseMax);

  Serial.println();
  Serial.println(F("STAGE 1 COMPLETE"));
  Serial.println();
  Serial.println();

  // Stage 2 = Set Channel center values (I've found they are not always equal to one half of max travel)
  // For Channel 3, if you have a 3-position switch, set it to center. If you have a 2 position switch, set it to ON
  // This routine will determine whether a 3 position switch is implemented or not.
  // ------------------------------------------------------------------------------------------------------------------------------------------>
  // Transition to Stage 2:
  // Off for two seconds, two slow blinks
  GreenLedOff();
  delay(2000);
  Serial.println(F("STAGE 2 - STORE CENTER VALUES"));
  PrintHorizontalLine();
  Serial.println(F("Place throttle and steering in NEUTRAL."));
  Serial.println(F("If Channel 3 is a 3-position switch, set it to CENTER."));
  Serial.println();
  GreenBlinkSlow(2);
  delay(2000);

  // Initialize some variables
  TotThrottlePulse = 0;
  TotTurnPulse     = 0;
  TotChannel3Pulse = 0;
  Count            = 0;

  // Start green LED blinking for stage two: two blinks every 1200 ms
  GreenBlinker = StartBlinking_ms(GreenLed_Pin, 2, 1200); // Blip the GreenLED twice every 1200ms
  StartWaiting_sec(6); // For the first bit of time we don't take any readings, this lets the user get the sticks centered
  Serial.println(F("Reading..."));
  Serial.println(F("Started reading"));
  do
  {
    delay(100);
    timer.run();
  } while (!TimeUp);
  StartWaiting_sec(4); // Now for the next four seconds we check the sticks
  do
  {
    Serial.print(".");
    TotThrottlePulse += pulseIn(ThrottleChannel_Pin, HIGH, ServoTimeout);
    TotTurnPulse     += pulseIn(SteeringChannel_Pin, HIGH, ServoTimeout);
    TotChannel3Pulse += pulseIn(Channel3_Pin, HIGH, ServoTimeout);
    // Increment reading count
    Count++;
    delay(10);
    // Refresh the timer
    timer.run();
  } while (!TimeUp); // Keep looping until time's up
  Serial.println(F("Started reading"));
  StopBlinking(GreenBlinker);

  // Finally we record our readings
  TempFloat           = (float)TotThrottlePulse / (float)Count;
  ThrottlePulseCenter = (int)lround(TempFloat);
  TempFloat           = (float)TotTurnPulse / (float)Count;
  TurnPulseCenter     = (int)lround(TempFloat);
  TempFloat           = (float)TotChannel3Pulse / (float)Count;
  Channel3PulseCenter = (int)lround(TempFloat);

  // Sanity check in case something weird happened (like Tx turned off during setup, or some channels disconnected)
  if ((ThrottlePulseCenter < MinPossiblePulse) || (ThrottlePulseCenter > MaxPossiblePulse))
  {
    ThrottlePulseCenter = TypicalPulseCenter;
  }
  if ((TurnPulseCenter < MinPossiblePulse) || (TurnPulseCenter > MaxPossiblePulse))
  {
    TurnPulseCenter = TypicalPulseCenter;
  }
  if ((Channel3PulseCenter < MinPossiblePulse) || (Channel3PulseCenter > MaxPossiblePulse))
  {
    Channel3PulseCenter = TypicalPulseCenter;
  }

  // Save values to EEPROM
  eeprom_write(ThrottlePulseCenter, E_ThrottlePulseCenter);
  eeprom_write(TurnPulseCenter, E_TurnPulseCenter);
  eeprom_write(Channel3PulseCenter, E_Channel3PulseCenter);

  Serial.println();
  Serial.println(F("Stage 2 Results - Pulse center values"));
  Serial.print(F("Throttle : "));
  Serial.println(ThrottlePulseCenter);
  Serial.print(F("Turn     : "));
  Serial.println(TurnPulseCenter);
  Serial.print(F("Ch3      : "));
  Serial.println(Channel3PulseCenter);

  Serial.println();
  Serial.println(F("STAGE 2 COMPLETE"));
  Serial.println();
  Serial.println();

  // Stage 3 = Set channel reversing.
  // ------------------------------------------------------------------------------------------------------------------------------------------>
  // Method used here is to ask the user to:
  // Hold throttle stick for full forward,
  // Hold steering wheel to full right,
  // Move Channel 3 to full ON
  // We take a string of readings and average them. We see if the pulse lengths are long or short, and knowing where the sticks are physically,
  // allows us to determine if we need to reverse any channels in software.

  // Transition to Stage 3:
  GreenLedOff();
  delay(2000);
  Serial.println(F("STAGE 3 - STORE CHANNEL DIRECTIONS"));
  PrintHorizontalLine();
  Serial.println(F("Hold trigger down (full forward), hold steering wheel full right, set Channel 3 to ON"));
  Serial.println();
  GreenBlinkSlow(3);
  delay(2000);

  // Initialize some variables
  TotThrottlePulse = 0;
  TotTurnPulse = 0;
  TotChannel3Pulse = 0;

  Count = 0;

  ThrottleChannelReverse = false;
  TurnChannelReverse = false;
  Channel3Reverse = false;

  // Start green LED blinking for stage three: three blinks every 1200 ms
  GreenBlinker = StartBlinking_ms(GreenLed_Pin, 3, 1200); // Blip the GreenLED three times every 1200ms
  StartWaiting_sec(6);                                // For the first bit of time we don't take any readings, this lets the user get the sticks centered
  Serial.println(F("Reading..."));
  Serial.println(F("Started reading"));
  while (!TimeUp)
  {
    timer.run();
  }

  StartWaiting_sec(4); // Now for the next four seconds we check the sticks
  do
  {
    Serial.print(".");
    // Add to our readings
    TotThrottlePulse += pulseIn(ThrottleChannel_Pin, HIGH, ServoTimeout);
    TotTurnPulse     += pulseIn(SteeringChannel_Pin, HIGH, ServoTimeout);
    TotChannel3Pulse += pulseIn(Channel3_Pin, HIGH, ServoTimeout);
    // Increment count
    Count++;
    delay(50);
    // Refresh the timer
    timer.run();
  } while (!TimeUp); // Keep looping until time's up
  Serial.println(F("Stopped reading"));
  StopBlinking(GreenBlinker);

  // Get the average of our readings
  TotThrottlePulse /= Count;
  TotTurnPulse     /= Count;
  TotChannel3Pulse /= Count;

  if (DEBUG)
  {
    Serial.print(F("Throttle Avg : "));
    Serial.println(TotThrottlePulse);
    Serial.print(F("Turn Avg     : "));
    Serial.println(TotTurnPulse);
    Serial.print(F("Channel3 Avg : "));
    Serial.println(TotChannel3Pulse);
  }

  // Decide if the channel is reversed, and do a sanity check as well
  // Throttle stick was held up, should have been long pulse. If not, reverse
  if ((TotThrottlePulse < 1300) && (TotThrottlePulse > MinPossiblePulse))
  {
    ThrottleChannelReverse = true;
  }
  // Turn stick was held right, should have been long pulse. If not, reverse
  if ((TotTurnPulse < 1300) && (TotTurnPulse > MinPossiblePulse))
  {
    TurnChannelReverse = true;
  }
  // We consider on to be high, should have been long pulse. If not, reverse
  if ((TotChannel3Pulse < 1300) && (TotChannel3Pulse > MinPossiblePulse))
  {
    Channel3Reverse = true;
  }

  // Save values to EEPROM
  eeprom_write(ThrottleChannelReverse, E_ThrottleChannelReverse);
  eeprom_write(TurnChannelReverse, E_TurnChannelReverse);
  eeprom_write(Channel3Reverse, E_Channel3Reverse);

  Serial.println();
  Serial.println(F("Stage 3 Results - Channel reversed"));
  Serial.print(F("Throttle : "));
  PrintTrueFalse(ThrottleChannelReverse);
  Serial.print(F("Turn     : "));
  PrintTrueFalse(TurnChannelReverse);
  Serial.print(F("Ch3      : "));
  PrintTrueFalse(Channel3Reverse);

  Serial.println();
  Serial.println(F("STAGE 3 COMPLETE"));
  Serial.println();
  Serial.println();

  // Transition to Stage End:
  GreenLedOff();
  delay(2000);
  GreenLedOn();
  delay(3000);
  GreenLedOff();

  // End Setup
  Serial.println(F("--- END SETUP ---"));
  Serial.println();
  RedLedOff();
}

// Smoothing code submitted by Wombii
// https://www.rcgroups.com/forums/showthread.php?1539753-Open-Source-Lights-Arduino-based-RC-Light-Controller/page57#post41145245
// Takes difference between current and old value, divides difference by none/2/4/8/16 and adds difference to old value (a quick and simple way of averaging)

int smoothThrottleCommand(int rawVal)
{
  static int SmoothedThrottle = 0;
  SmoothedThrottle = SmoothedThrottle + ((rawVal - SmoothedThrottle) >> SmoothingStrength);
  return SmoothedThrottle;
}

int smoothSteeringCommand(int rawVal)
{
  static int SmoothedSteer = 0;
  SmoothedSteer = SmoothedSteer + ((rawVal - SmoothedSteer) >> SmoothingStrength);
  return SmoothedSteer;
}

int smoothChannel3Command(int rawVal)
{
  static int SmoothedCh3 = 0;
  SmoothedCh3 = SmoothedCh3 + ((rawVal - SmoothedCh3) >> SmoothingStrength);
  return SmoothedCh3;
}