#include <Arduino.h>
#include <avr/pgmspace.h>

extern const int8_t NA;
extern const int8_t BLINK;
extern const int8_t FASTBLINK;
extern const int8_t SOFTBLINK;
extern const int8_t DIM;
extern const int8_t FADEOFF;
extern const int8_t XENON;
extern const int8_t BACKFIRE;

extern const int8_t ON;
extern const int8_t OFF;

extern const int8_t YES;
extern const int8_t NO;

extern const int Schemes[NumSchemes][NumLights][NumStates] PROGMEM;