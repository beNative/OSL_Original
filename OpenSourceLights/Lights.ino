// ------------------------------------------------------------------------------------------------------------------------------------------------>
// SETUPLIGHTS - This assigns the various settings to each of the light states. Run once on startup, and each time the scheme is changed.
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void SetupLights(int WhatScheme)
{
    int i;
    int j;

    for (i=0; i<NumLights; i++)
    {
        for (j=0; j<NumStates; j++)
        {
            LightSettings[i][j] = pgm_read_word_near(&(Schemes[WhatScheme-1][i][j]));    // WhatScheme is minus -1 because Schemes are zero-based. We let the user use
                                                                                         // one-based numbers for convenience
            PriorLightSetting[i][j] = LightSettings[i][j];                               // Save this to "prior" too. In case we temporarily change a state,
        }                                                                                // this lets us know what we should revert back to.
    }
    return;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// SETLIGHTS - the main function which assigns the appropriate setting to each light based on the current actual drive mode (different from commanded drive mode)
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void SetLights(int DriveMode)
{
    int SaveSetting[NumLights];
    int j;

    // Loop through each light, assign the setting appropriate to its state
    for (j=0; j<NumLights; j++)
    {
        // We will use the temporary variable SaveSetting to assign the setting for this light.
        // A light could have multiple settings apply at one time, be we can only set it to one thing.
        // Therefore each setting is ranked by importance. If multiple settings apply to a light,
        // the setting applied LAST will be the one used (each check can overwrite the prior one).
        // You can re-order the checks below, the least important should come first, and most important last.


        // Least important - does this light have a setting related to Channel 3?
        // --------------------------------------------------------------------------------------------------->>
        SaveSetting[j] = LightSettings[j][Channel3];


        // Next - does this light have a setting related to Drive Mode? (Forward, reverse, stop)
        // --------------------------------------------------------------------------------------------------->>
        switch (DriveMode) {
            case FWD:
                if (LightSettings[j][StateFwd]  != NA) { SaveSetting[j] = LightSettings[j][StateFwd]; }
                break;
            case REV:
                if (LightSettings[j][StateRev]  != NA) { SaveSetting[j] = LightSettings[j][StateRev]; }
                break;
            case STOP:
                // We have two stop states:
                // StateStop occurs when the vehicle stops
                // StateStopDelay occurs when the vehicle has been stopped for LongStopTime_mS and will supersede StateStop when it occurs (if not NA)
                if (LightSettings[j][StateStop] != NA) { SaveSetting[j] = LightSettings[j][StateStop]; }
                if (LightSettings[j][StateStopDelay] != NA && StoppedLongTime == true) { SaveSetting[j] = LightSettings[j][StateStopDelay]; }
                break;
        }


        // Next - does this light come on during deceleration (probably Backfiring?)
        // --------------------------------------------------------------------------------------------------->>
        if (canBackfire)
        {
//            if (LightSettings[j][StateDecel] != NA) { SaveSetting[j] = BACKFIRE; } // Override setting - we assume the only setting they want during decel is BACKFIRE
            if (LightSettings[j][StateDecel] != NA) { SaveSetting[j] = LightSettings[j][StateDecel]; } // Or we can allow any setting during deceleration
        }

        // Next -
        // --------------------------------------------------------------------------------------------------->>
        if (Overtaking)
        {
            if (LightSettings[j][StateAccel] != NA) { SaveSetting[j] = LightSettings[j][StateAccel]; }
        }

        // Next - does this light come on during braking?
        // --------------------------------------------------------------------------------------------------->>
        if (Braking)
        {
            if (LightSettings[j][StateBrake] != NA) { SaveSetting[j] = LightSettings[j][StateBrake]; }
        }


        // Next - does this light come on during turns?
        // --------------------------------------------------------------------------------------------------->>
        if (TurnCommand > 0 || TurnSignalOverride > 0)    // Right Turn
        {
            // If we have a blink command on right turn, and if we have the BlinkTurnOnlyAtStop = true,
            // then we only appy the turn signal if we are stopped AND if the turn signal delay has expired (TurnSignal_Enable = true)
            if ((LightSettings[j][StateRT] == BLINK || LightSettings[j][StateRT] == SOFTBLINK) && (BlinkTurnOnlyAtStop == true))
            {
                if ((DriveMode == STOP) && (TurnSignal_Enable == true)) { SaveSetting[j] = LightSettings[j][StateRT]; }
            }
            // Same as above except for all other settings under turn
            else if (LightSettings[j][StateRT] != NA && AllTurnSettingsMatch == true )
            {
                if ((DriveMode == STOP) && (TurnSignal_Enable == true)) { SaveSetting[j] = LightSettings[j][StateRT]; }
            }
            // Otherwise if it is any other setting, or if the BlinkTurnOnlyAtStop flag and the AllTurnSettingsMatch are not true, then we apply the setting normally
            else if (LightSettings[j][StateRT] != NA) { SaveSetting[j] = LightSettings[j][StateRT]; }
        }
        if (TurnSignalOverride > 0) // Artificial Right Turn
        {
            // In this case we want to artificially create a turn signal even though the wheel may or may not be turned.
            // We ignore driving state or TurnSignal_Enable state
            if (LightSettings[j][StateRT] == BLINK || LightSettings[j][StateRT] == SOFTBLINK) { SaveSetting[j] = LightSettings[j][StateRT]; }
            // We may also want to artificially create any setting assigned to the turn state
            else if (AllTurnSettingsMatch)                                                    { SaveSetting[j] = LightSettings[j][StateRT]; }
        }

        if (TurnCommand < 0 || TurnSignalOverride < 0)    // Left Turn
        {
            // If we have a blink command on left turn, and if we have the BlinkTurnOnlyAtStop = true,
            // then we only appy the turn signal if we are stopped AND if the turn signal delay has expired (TurnSignal_Enable = true)
            if ((LightSettings[j][StateLT] == BLINK || LightSettings[j][StateLT] == SOFTBLINK) && (BlinkTurnOnlyAtStop == true))
            {
                if ((DriveMode == STOP) && (TurnSignal_Enable == true)) { SaveSetting[j] = LightSettings[j][StateLT]; }
            }
            // Same as above except for all other settings under turn
            else if (LightSettings[j][StateLT] != NA && AllTurnSettingsMatch == true )
            {
                if ((DriveMode == STOP) && (TurnSignal_Enable == true)) { SaveSetting[j] = LightSettings[j][StateLT]; }
            }
            // Otherwise if it is any other setting, or if the BlinkTurnOnlyAtStop flag and the AllTurnSettingsMatch are not true, then we apply the setting normally
            else if (LightSettings[j][StateLT] != NA) { SaveSetting[j] = LightSettings[j][StateLT]; }
        }
        if (TurnSignalOverride < 0) // Artificial Left Turn
        {
            // In this case we want to artificially create a turn signal even though the wheel may or may not be turned.
            // We ignore driving state or TurnSignal_Enable state
            if (LightSettings[j][StateLT] == BLINK || LightSettings[j][StateLT] == SOFTBLINK) { SaveSetting[j] = LightSettings[j][StateLT]; }
            // We may also want to artificially create any setting assigned to the turn state
            else if (AllTurnSettingsMatch)                                                    { SaveSetting[j] = LightSettings[j][StateLT]; }
        }

        // Light "j" now has a single setting = SaveSetting[j]
        // We call the function that will set this light to that setting
        SetLight(j, SaveSetting[j]);
    }

    // Now we have done looping through all the lights, and setting them all.

    // There is an option in UserConfig.h to have the board LEDs reflect car movement, this
    // can sometimes be useful for debugging.

    // If moving forward, green led is on, if moving backwards, red led is on.
    // If stopped, both are off.
    // If braking, both are on.
    // If right turn, green LED blinks quickly
    // If left turn, red LED blinks quickly
    if (LED_DEBUG)
    {
        if (DriveMode == FWD)  { digitalWrite(RedLED , LOW ) ; digitalWrite(GreenLED, HIGH) ; }
        if (DriveMode == REV)  { digitalWrite(RedLED , HIGH) ; digitalWrite(GreenLED, LOW ) ; }
        if (DriveMode == STOP) { digitalWrite(RedLED , LOW ) ; digitalWrite(GreenLED, LOW ) ; }
        if (Braking == true)   { digitalWrite(RedLED , HIGH) ; digitalWrite(GreenLED, HIGH) ; }
        if (TurnCommand > 0)   { GreenBlink(); }    // Right turn
        if (TurnCommand < 0)   { RedBlink();   }    // Left turn
    }

}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// DUMPLIGHTSCHEMETOSERIAL - Show each setting for each state for each light in tabular format out the serial port
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void DumpLightSchemeToSerial(int WhatScheme)
{
    if (WhatScheme <= NumSchemes)
    {   // Schemes are zero-based, so if they pass Scheme 1 what we really want to show is Scheme 0
        WhatScheme -= 1;
        Serial.print(F("SCHEME: "));
        Serial.print(WhatScheme+1);
        Serial.println();
        for (int i=0; i<NumLights; i++)
        {
            Serial.print(F("Light #"));
            Serial.print(i+1);
            Serial.print(F(" States: "));
            for (int j=0; j<NumStates; j++)
            {
                int MySetting = pgm_read_word_near(&(Schemes[WhatScheme][i][j]));
                Serial.print(MySetting, DEC);
                Serial.print(F("  "));
            }
            Serial.println();
        }
    }
    return;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// SETLIGHT - This sets an individual light to a specific setting - Wombii modified
// FADEDISABLED:'0' FADEQUICK:'1' FADEUP:'2' FADEDOWN:'3' FADEBOTH:'4' FADEXENON:'5'
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void SetLight(int WhatLight, int WhatSetting)
{
  byte WantedLightState = 0;
  byte WantedLightValue = 0;
  byte WantedFade = '0';
  byte activeLightValue = 0;

  static byte pinIsFlashing[NumLights] = {0,0,0,0,0,0,0,0}; //Helper to reset the phase to make sure we start a flash session ON with no delay

  switch (WhatSetting)
  {
    case ON:
      WantedLightState = 1;
      WantedLightValue = 255;
      WantedFade = '0';
      pinIsFlashing[WhatLight] = 0;
      break;

    case OFF:
      WantedLightState = 0;
      WantedLightValue = 0;
      WantedFade = '0';
      pinIsFlashing[WhatLight] = 0;
      break;

    case BLINK:
      if (pinIsFlashing[WhatLight] == 0) {
        specialTimingArray[0][1] = 255;
      }
      WantedLightState = flashingFunctionStartActiveSimple(0,specialTimingArray[0]);
      WantedLightValue = WantedLightState * 255;
      WantedFade = '0';
      if (LightSettings[WhatLight][Channel3] == DIM) {
        if(WantedLightState == 0) {
          WantedLightValue = ActualDimLevel;
        }
      }
      pinIsFlashing[WhatLight] = 1;
      break;

    case SOFTBLINK:
      if (pinIsFlashing[WhatLight] == 0) {
        specialTimingArray[1][1] = 255;             //If new flash session, reset phase
      }
      WantedLightState = flashingFunctionStartActiveSimple(1,specialTimingArray[1]);
      WantedLightValue = WantedLightState * 255;
      WantedFade = '4';
      if (LightSettings[WhatLight][Channel3] == DIM) {
        if(WantedLightState == 0) {
          WantedLightValue = ActualDimLevel;
        }
      }
      pinIsFlashing[WhatLight] = 1;                                                   //Currently in a flashing session, leave phase alone
      break;

    case FASTBLINK:
      if (pinIsFlashing[WhatLight] == 0)  specialTimingArray[2][1] = 255;
      WantedLightState = flashingFunctionStartActiveSimple(2,specialTimingArray[2]);
      WantedLightValue = WantedLightState*255;
      WantedFade = '0';
      if (LightSettings[WhatLight][Channel3] == DIM)
      {
        if(WantedLightState == 0)
          WantedLightValue = ActualDimLevel;
      }
      pinIsFlashing[WhatLight] = 1;
      break;

    case DIM:
      WantedLightValue = ActualDimLevel;
      WantedFade = '0';
      pinIsFlashing[WhatLight] = 0;
      break;

    case BACKFIRE: //Unchanged
      LightBackfire(WhatLight);
      pinIsFlashing[WhatLight] = 0;
      break;

    case XENON:
      WantedLightState = 1;
      WantedLightValue = 255;
      WantedFade = '5';
      pinIsFlashing[WhatLight] = 0;
      break;

    case FADEOFF:
      WantedLightState = 0;
      WantedLightValue = 0;
      WantedFade = '3';
      pinIsFlashing[WhatLight] = 0;
      break;
  }

  activeLightValue = SimpleFader(WhatLight, WantedFade, WantedLightValue);

  //Actually write to the pins: (Could do all with analogWrite, but can do the digitalWrite parts to possibly save some time)
  if (activeLightValue == 0)
      digitalWrite(LightPin[WhatLight],LOW);
  else if (activeLightValue == 255)
      digitalWrite(LightPin[WhatLight],HIGH);
  else
      analogWrite(LightPin[WhatLight],activeLightValue);
}


// ------------------------------------------------------------------------------------------------------------------------------------------------>
// FADING FUNCTION - Wombii
// Deals with all fading. Tell it which pin, the wanted fade setting and the end target.
// This can be used transparently by using fade setting '0' to have output directly match target value.
// ------------------------------------------------------------------------------------------------------------------------------------------------>
byte SimpleFader(byte currentPin, byte wantedFadeSetting, byte wantedOutput)
{
    #define FADEDISABLED  '0'
    #define FADEQUICK     '1'
    #define FADEUP        '2'
    #define FADEDOWN      '3'
    #define FADEBOTH      '4'
    #define FADEXENON     '5'
    //5+1 and 5+2 is reserved for xenon stages. continue with fade sinewave at 8 when implemented.

    static byte fadeValueArray[NumLights] = {0,0,0,0,0,0,0,0};      //Stores the current pwm value per pin
    static byte previousFadeSetting[NumLights] = {0,0,0,0,0,0,0,0}; //Stores the previously used fade setting

    const byte fadeUpSlowSpeed = 10;  //User setting?

    int lastOutput = 0;               //The calculated output from the previous loop
    int calculatedOutput = 0;         //The output after fading calculation

    lastOutput = fadeValueArray[currentPin];

    byte fadeSetting = wantedFadeSetting;

    //If I remember correctly, this helps the xenon sequencing work:
    if (wantedOutput < lastOutput){
      if(previousFadeSetting[currentPin] >= '5'){//Fixes xenon bug.
        fadeSetting = previousFadeSetting[currentPin];
      }
    }
    else
    {
      fadeSetting = wantedFadeSetting;
      if (previousFadeSetting[currentPin] < FADEXENON)
        previousFadeSetting[currentPin] = wantedFadeSetting;
    }

    switch (fadeSetting)
    {
      case FADEQUICK: //Fast bug less smooth fade (both up and down)
        previousFadeSetting[currentPin] = FADEQUICK;
        if (wantedOutput > lastOutput){
          calculatedOutput = min(lastOutput + 20, wantedOutput);}
        else if (wantedOutput < lastOutput){
          calculatedOutput = max(lastOutput - 20, wantedOutput);}
        else{
          calculatedOutput = wantedOutput;}
        break;

      case FADEUP: //Fade when going up, no fade when going down.
        previousFadeSetting[currentPin] = FADEUP;
        if (wantedOutput > lastOutput)
          calculatedOutput = min(lastOutput + fadeUpSlowSpeed, wantedOutput);
        else
          calculatedOutput = wantedOutput;
        break;

      case FADEDOWN: //Fade when going down, no fade when going up. //Currently multiplies instead of subtracts, to get a better curve
        previousFadeSetting[currentPin] = FADEDOWN;
        if (wantedOutput < lastOutput)
          calculatedOutput = max(0.9*lastOutput,wantedOutput);
        else
          calculatedOutput = wantedOutput;
        break;

      case FADEBOTH: // Fade both up and down //optimize here?
        previousFadeSetting[currentPin] = FADEBOTH;
        if (wantedOutput > lastOutput)
          calculatedOutput = min(lastOutput + fadeUpSlowSpeed, wantedOutput);
        else if (wantedOutput < lastOutput)
          calculatedOutput = max(0.95*lastOutput,wantedOutput);// - fadeValue, wantedOutput);
        else
        {
          calculatedOutput = wantedOutput;
        }
        break;

      //The xenon fading works in 3 stages.
      //1: Starting from off, spike the value to a medium-high value.
      //2: Go quickly down to a low value.
      //3: Slowly fade up to the wanted output value (probably full, but supports any value).
      // When turning off, fade out.
      case FADEXENON:
      case FADEXENON+1:
      case FADEXENON+2:

        if (wantedOutput > lastOutput)
        {
          byte flashMax = wantedOutput - (wantedOutput>>2);
          if ((previousFadeSetting[currentPin] == (FADEXENON + 0)) || (previousFadeSetting[currentPin] == 0))
          {
            calculatedOutput = lastOutput + (wantedOutput>>2);// = min(lastOutput + 30,flashMax); // last variable is the max value for the ignition flash// calculated is now 60 instead of 40 on run 2 if wanted is 40? FIX!
            //calculatedOutput = min(calculatedOutput,wantedOutput); //fixed
            if (calculatedOutput >= flashMax)
            {
              previousFadeSetting[currentPin]=FADEXENON + 1;
              calculatedOutput = flashMax;
            }
          }
          else if (previousFadeSetting[currentPin] == (FADEXENON + 1))
          {
            previousFadeSetting[currentPin]= FADEXENON + 2;
            calculatedOutput = 1;
          }
          else if (previousFadeSetting[currentPin] == (FADEXENON + 2))
          {
            calculatedOutput = lastOutput + 1; //min(lastOutput + 1,wantedOutput);
            if (calculatedOutput >= wantedOutput)
            {
              previousFadeSetting[currentPin] = FADEXENON;
            }
          }
        }
        else if (wantedOutput < lastOutput)
        {
          calculatedOutput = max(0.9*lastOutput,wantedOutput); //change to sine wave fade?
          previousFadeSetting[currentPin] = FADEXENON;
        }
        else if (wantedOutput == lastOutput)
        {
          calculatedOutput = wantedOutput;
          previousFadeSetting[currentPin] = FADEXENON;
        }
        break;
      default:
        calculatedOutput = wantedOutput;
        previousFadeSetting[currentPin] = 0;
        break;
    }

    fadeValueArray[currentPin] = calculatedOutput;
    return calculatedOutput;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// FLASHING FUNCTION - Wombii
// Deals with flashing. Makes a square wave with custom cycle time and time ON.
// tempArray[4] would be of format: {ignored,phase,cycletime,timeOn}
// ------------------------------------------------------------------------------------------------------------------------------------------------>

// For funcions that should go active immediately when turned on, like turn signals. moves the phase to the current runcount before flashing.
//REQUIRES RESETTING OF PHASE
// Visualization attempt:
//   Generate a square wave with amplitude 1.
//   numberOfFrames is the wavelength/period/cycle time.
//   numberOfActiveFrames is the High pulse width.
//   phase is the delay before the rising edge.
byte flashingFunctionStartActiveSimple ( byte functionNumber, byte tempArray[4]) // -Wombii
{
  byte phase = tempArray[1];
  byte numberOfFrames = tempArray[2];
  byte numberOfActiveFrames = tempArray[3];
  byte flasherState = 0;

  //set phase if this is a new blinking session to always start ON
  if (phase == 255) {
    phase = numberOfFrames - (runCount % numberOfFrames );
    specialTimingArray[functionNumber][1] = phase;
  }

  //split up the infinite number line of loops into reasonable chunks.
  //numberOfFrames = number of loops per light cycle
  //numberOfActiveFrames = number of loops per light cycle that should be ON.
  //phase = number of frames to wait before starting the ON part.
  if ((runCount + phase)%numberOfFrames < numberOfActiveFrames ) {
    flasherState = 1;
  }

  return flasherState;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// TURNONLIGHT - Turns a light on
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void TurnOnLight(int WhatLight)
{
  digitalWrite(LightPin[WhatLight], HIGH);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// TURNOFFLIGHT - Turns a light off
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void TurnOffLight(int WhatLight)
{
  digitalWrite(LightPin[WhatLight], LOW);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// LIGHTBACKFIRE - briefly and randomly light a led
// All credit for backfiring code goes to Sergio Pizzotti
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void LightBackfire(int WhatLight)
{
  // Has enough time passed to flicker the backfire LED?
  if (canBackfire) {
    // Save time for next check
    if(millis() - Backfire_millis > backfire_interval) {
      Backfire_millis = millis();
      // Change state of backfire LED
      for (int i=0; i<NumLights; i++) {
        if (LightSettings[i][StateDecel] == BACKFIRE) {ReverseLight(i); }
      }
      // Calculate new random interval for the next flicker
      backfire_interval = random(BF_Short, BF_Long);
    }
  }
}

void BackfireOff()
{
  // Time up - stop backfire effect
  canBackfire = false;
  // Reset the random backfire timeout for the next event
  backfire_timeout = BF_Time + random(BF_Short, BF_Long);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// OVERTAKEOFF - Overtaking time is up
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void OvertakeOff()
{
  // Time up - stop the overtake effects
  Overtaking = false;
  // The overtaking effect can cause a Xenon effect to re-start, so in the event a Xenon effect is defined for this same light,
  for (int i=0; i<NumLights; i++) {
    // we go ahead and flag it complete.
    if (LightSettings[i][StateAccel] != NA) {
      Xenon_EffectDone[i] = 1;
    }
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// TURN SIGNAL - Artificial turn signal cancel
// ------------------------------------------------------------------------------------------------------------------------------------------------>
// If TurnSignalOverride is not zero, then it acts as an artificial turn signal command, keeping the turn signals on even after the car has begun
// to move forward. But it only does so for a brief period of time (set by the user in TurnFromStartContinue_mS in UserConfig.h). When that time
// runs out this function is called which sets TurnSignalOverride back to 0.
void ClearBlinkerOverride(void)
{
  TurnSignalOverride = 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// FASTBLINKLIGHT - This blinks a light at the fast rate
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void FastBlinkLight(int WhatLight)
{
  FastBlinker ? TurnOnLight(WhatLight) : TurnOffLight(WhatLight);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// FIXDIMLEVEL - Sets the ActualDimLevel and prevents the bug that occurs if the User strangely set DimLevel to less than 2 (unlikely)
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void FixDimLevel()
{
  if (DimLevel < 2) {
    ActualDimLevel = 2;
  }
  else {
    ActualDimLevel = DimLevel;
  }
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// TWINKLELIGHTS - Sets all lights to FastBlink for Seconds seconds
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void TwinkleLights(int Seconds)
{
  StartWaiting_sec(Seconds);
  do {
    for (int j=0; j<NumLights; j++) {
      FastBlinkLight(j);
    }
    timer.run();
  }
  while(!TimeUp);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------>
// BLINKALLLIGHTS - Blinks every single light, onboard and off, HowManyTimes. This is used as an indicator during Change-Scheme-Mode
// ------------------------------------------------------------------------------------------------------------------------------------------------>
void BlinkAllLights(int HowManyTimes)
{
  if (Blinking) {
    if (State) {
      if (!PriorState) {
        // Turn everything on
        GreenLedOn();
        RedLedOn();
        for (int j=0; j<NumLights; j++){
          TurnOnLight(j);
        }
        PriorState = true;
        TimesBlinked += 1;
        timer.setTimeout(130, BlinkOn);
      }
    }
    else {
      if (PriorState) {
        // Turn everything off
        GreenLedOff();
        RedLedOff();
        for (int j=0; j<NumLights; j++)
        {   TurnOffLight(j); }
        PriorState = false;
        BlinkOffID = timer.setTimeout(160, BlinkOff);
      }

      if (TimesBlinked >= HowManyTimes) {
        TimesBlinked = 0;
        Blinking = false;
        timer.setTimeout(1000, BlinkWait);
        timer.deleteTimer(BlinkOffID);
      }
    }
  }
}

void BlinkOn()
{
  Blinking = true;
  State = false;
  PriorState = true;
}

void BlinkOff()
{
  Blinking = true;
  State = true;
  PriorState = false;
}

void BlinkWait()
{
  Blinking = true;
  State = true;
  PriorState = false;
}

void ReverseLight(int WhatLight)
{
  int OppositeVal;
  // If light is on, turn it off. But if light is off, turn it on.
  OppositeVal = !digitalRead(LightPin[WhatLight]);
  digitalWrite(LightPin[WhatLight], OppositeVal);
  PWM_Step[WhatLight] = OppositeVal;
}
