#ifndef UTILITIES_H
#define UTILITIES_H
  #include <Arduino.h>

  // EEPROM defines
  #define eeprom_read_to(dst_p, eeprom_field, dst_size) eeprom_read_block(dst_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(dst_size, sizeof((__eeprom_data*)0)->eeprom_field))
  #define eeprom_read(dst, eeprom_field) eeprom_read_to(&dst, eeprom_field, sizeof(dst))
  #define eeprom_write_from(src_p, eeprom_field, src_size) eeprom_write_block(src_p, (void *)offsetof(__eeprom_data, eeprom_field), MIN(src_size, sizeof((__eeprom_data*)0)->eeprom_field))
  #define eeprom_write(src, eeprom_field) { typeof(src) x = src; eeprom_write_from(&x, eeprom_field, sizeof(x)); }

  // Useful functions
  #define MIN(x,y) ( x > y ? y : x )
  #define MAX(x,y) ( x > y ? x : y )

  // EEPROM
  // ------------------------------------------------------------------------------------------------------------------------------------------------>

  // Variables
  extern int ThrottlePulseMin;
  extern int ThrottlePulseMax;
  extern int ThrottlePulseCenter;
  extern int TurnPulseMin;
  extern int TurnPulseMax;
  extern int TurnPulseCenter;
  extern int Channel3PulseMin;
  extern int Channel3PulseMax;
  extern int Channel3PulseCenter;
  extern boolean ThrottleChannelReverse;
  extern boolean TurnChannelReverse;
  extern boolean Channel3Reverse;
  extern int CurrentScheme;

  const long EEPROM_Init         = 0xAA43;                // Change this any time the EEPROM content changes
  struct __eeprom_data {                                  // __eeprom_data is the structure that maps all of the data we are storing in EEPROM
    long E_InitNum;                                       // Number that indicates if EEPROM values have ever been initialized
    int E_ThrottlePulseMin;
    int E_ThrottlePulseMax;
    int E_ThrottlePulseCenter;
    int E_TurnPulseMin;
    int E_TurnPulseMax;
    int E_TurnPulseCenter;
    int E_Channel3PulseMin;
    int E_Channel3PulseMax;
    int E_Channel3PulseCenter;
    boolean E_ThrottleChannelReverse;
    boolean E_TurnChannelReverse;
    boolean E_Channel3Reverse;
    int E_CurrentScheme;
  };

  void Initialize_EEPROM();
  void Load_EEPROM();
  void SaveScheme_To_EEPROM();

  // Serial Print Functions
  void PrintSpaceDash();
  void PrintHorizontalLine();
  void PrintTrueFalse(boolean boolVal);
#endif // ndef UTILITIES_H
