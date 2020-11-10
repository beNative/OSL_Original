// ------------------------------------------------------------------------------------------------------------------------------------------------>
// THIS IS WHERE YOU DETERMINE THE SETTINGS FOR YOUR LIGHTS
// ------------------------------------------------------------------------------------------------------------------------------------------------>

// SETUP INSTRUCTIONS

// EXPLANATION OF STATES
// ------------------------------------------------------------------------------------------------------------------------------------------------>
// Each light can respond to nine different conditions, or states. These are:
// - Channel 3 - Position 1
// - Channel 3 - Position 2
// - Channel 3 - Position 3    (This is the middle position if using a 3-position switch)
// - Channel 3 - Position 4
// - Channel 3 - Position 5    (This is position  3  if using a 3-position switch, or position  2  if using a 2-position switch)
// - Forward
// - Reverse
// - Stop
// - Brake
// - Right Turn
// - Left Turn
// - Accelerating - this is a subset of the Forward state, and occurs during heavy acceleration (defined in UserConfig)
// - Decelerating - this is a subset of the Forward state, and occurs during deceleration (defined in UserConfig)
//
// State Notes:
// - At the very least, you must plug in the Throttle channel. Steering and Channel 3 are optional
// - If you do not plug in a steering channel, then obviously you will never encounter the Right Turn & Left Turn States
// - If you do not plug in a third channel, the program will only use the values specified in Position 1 as the default. In other words, it acts
//   as if you have a one-position switch that is always in Position 1
// - If your third channel is only a 2-position switch, the program will switch between Position 1 and Position 5
// - If your third channel is a 3-position switch, you will be able to switch between Positions 1, 3, and 5
// - If your third channel is a 5-position switch, you will be able to select all five Positions
// - For the third channel to operate correctly, and in fact, for all channels to operate correctly, you need to run through the Radio Setup once.

// EXPLANATION OF SETTINGS
// ------------------------------------------------------------------------------------------------------------------------------------------------>
// For each light, you need to specify what will happen in each state - this is called the setting. The possible settings are:
// - ON
// - OFF
// - FADEOFF        * Fades off slowly
// - NA
// - BLINK            Will blink at the rate set in UserConfig.h (BlinkInterval)
// - SOFTBLINK        Will blink at the rate set in UserConfig.h (BlinkInterval), but the light will fade-in and fade-out. The timing of the fade can be adjusted, see UserConfig
// - FASTBLINK        Will blink at the fast rate set in UserConfig.h
// - DIM            * Will dim to the level set in UserConfig.h
// - XENON          * Turns a light on but with a special effect that looks like a xenon bulb turning on
// - BACKFIRE         Special effect that blinks a light randomly for a short period of time (only works under Decelerating state - use for tailpipe LEDs)
//
// Settings Notes:
// - For the positions determined by Channel 3, it is best to specify an explicit setting, in other words, you probably don't want any of them to be NA -
//   they should be ON, OFF, whatever.
// - The opposite is true for the other states - if you don't want anything to happen during the Forward state for example, set it to NA, not OFF.
//   Otherwise, your light will turn off when going forward.
// - Some settings require a special feature known as PWM. These are marked above with an asterisks (*). Not all of the lights on the board are capable of implementing PWM,
//   only the first 6 sockets. If you look at the underside of the physical board, these lights are marked with an asterisks (*). If you want to use these special settings,
//   they must be on lights 1-6. Otherwise if you specify one of these settings on lights 7 or 8, the program will simply turn them OFF instead.

// EXPLANATION OF SCHEMES
// ------------------------------------------------------------------------------------------------------------------------------------------------>
// A Scheme defines every setting for every light in every state. You may have as many Schemes as code space allows. Two are provided below to start,
// if you want more, add them below, but remember to also update the NumSchemes variable in AA_UserConfig.h.

// HOW TO SETUP YOUR LIGHTS
// ------------------------------------------------------------------------------------------------------------------------------------------------>
// Below you will see the lighting schemes. Each Scheme has a single row for each of the eight lights. The columns represent the states. The values
// in the individual tables represent the settings for that light at that state.
//
// OK, YOU'RE READY. TRY NOT TO MESS UP THE LAYOUT. JUST CHANGE THE SETTINGS. REMEMBER THE POSSIBLE VALUES:
// ON, OFF, FADEOFF, BLINK, FASTBLINK, DIM, XENON, FADEOFF, NA
// IF CHANNEL 3 is only 3-position switch, values in Pos2 and Pos4 will be ignored (just use Pos1, Pos3, Pos5)

const int Schemes[NumSchemes][NumLights][NumStates] PROGMEM =
{
  { // SCHEME 1: DEFAULT
    // POS1  POS2  POS3  POS4  POS5  FWD   REV   STP   STPD  BRAKE RIGHT LEFT  ACCEL DECEL
    // ---------------------------------------------------------------------------------------------------------------
    {  ON,   ON,   ON,   ON,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }, // L1~ Low/High beam
    {  ON,   ON,   ON,   ON,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }, // L2  Parking lights
    {  ON,   ON,   ON,   ON,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }, // L3  NC
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   BLINK,NA,   NA  }, // L4~ Left turn signals
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   BLINK,NA,   NA,   NA  }, // L5~ Right turn signals
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }, // L6  Reverse lights
    {  DIM,  DIM,  DIM,  DIM,  DIM,  NA,   NA,   ON,   DIM,  ON,   NA,   NA,   NA,   ON  }, // L7~ Brake/Tail lights
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   ON,   OFF,  ON,   NA,   NA,   NA,   ON  }  // L8  Brake lights
  },
  { // SCHEME 2: Same as default, but CH3 is used to switch hazard lights
    // POS1  POS2  POS3  POS4  POS5  FWD   REV   STP   STPD  BRAKE RIGHT LEFT  ACCEL DECEL
    // ---------------------------------------------------------------------------------------------------------------
    {  ON,   ON,   ON,   ON,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }, // L1~ Low/High beam
    {  ON,   ON,   ON,   ON,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }, // L2  Parking lights
    {  ON,   ON,   ON,   ON,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }, // L3  NC
    {  OFF,  OFF,  OFF,  BLINK,BLINK,NA,   NA,   NA,   NA,   NA,   NA,   BLINK,NA,   NA  }, // L4~ Left turn signals
    {  OFF,  OFF,  OFF,  BLINK,BLINK,NA,   NA,   NA,   NA,   NA,   BLINK,NA,   NA,   NA  }, // L5~ Right turn signals
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }, // L6  Reverse
    {  DIM,  DIM,  DIM,  DIM,  DIM,  NA,   NA,   ON,   DIM,  ON,   NA,   NA,   NA,   ON  }, // L7~ Brake/Tail lights
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   ON,   OFF,  ON,   NA,   NA,   NA,   ON  }  // L8  Brake lights
  },
  { // SCHEME 3: LANDROVER MN MODEL
    // POS1  POS2  POS3  POS4  POS5  FWD   REV   STP   STPD  BRAKE RIGHT LEFT  ACCEL DECEL
    // ---------------------------------------------------------------------------------------------------------------
    {  DIM,  DIM,  DIM,  DIM,  DIM,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L1~
    {  ON,   ON,   ON,   ON,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L2
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   ON,   ON,   NA,   NA,   NA,   NA,   NA  },  // L3
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   BLINK,NA,   NA  },  // L4~ Left
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   BLINK,NA,   NA,   NA  },  // L5~ Right
    {  DIM,  DIM,  DIM,  DIM,  DIM,  NA,   NA,   ON,   DIM,  ON,   NA,   NA,   NA,   NA  },  // L6  Brake
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L7~
    {  DIM,  DIM,  DIM,  DIM,  DIM,  DIM,  DIM,  DIM,  DIM,  DIM,  DIM,  DIM,  DIM,  DIM },  // L8
  },
  { // SCHEME 4: BLANK
    // POS1  POS2  POS3  POS4  POS5  FWD   REV   STP   STPD  BRAKE RIGHT LEFT  ACCEL DECEL
    // ---------------------------------------------------------------------------------------------------------------
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L1~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L2
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L3
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   BLINK,NA,   NA  },  // L4~ Left
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   BLINK,NA,   NA,   NA  },  // L5~ Right
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   ON,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L6  Reverse
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L7~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   ON,   OFF,  ON,   NA,   NA,   NA,   ON  }   // L8  Brake
  },
  { // SCHEME 5: BLANK
    // POS1  POS2  POS3  POS4  POS5  FWD   REV   STP   STPD  BRAKE RIGHT LEFT  ACCEL DECEL
    // ---------------------------------------------------------------------------------------------------------------
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L1~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L2
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L3
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L4~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L5~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L6
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L7~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }   // L8
  },
  { // SCHEME 6: BLANK
    // POS1  POS2  POS3  POS4  POS5  FWD   REV   STP   STPD  BRAKE RIGHT LEFT  ACCEL DECEL
    // ---------------------------------------------------------------------------------------------------------------
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L1~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L2
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L3
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L4~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L5~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L6
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  },  // L7~
    {  OFF,  OFF,  OFF,  OFF,  OFF,  NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA,   NA  }   // L8
  }
};