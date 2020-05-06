#ifndef EEPROM_HELPERS_H
#define EEPROM_HELPERS_H

#include <Arduino.h>

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

// EEPROM
// ------------------------------------------------------------------------------------------------------------------------------------------------>
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

#endif // ndef EEPROM_HELPERS_H
