#include <Arduino.h>
#include <avr/pgmspace.h>

extern const uint16_t NA;
extern const uint16_t BLINK;
extern const uint16_t FASTBLINK;
extern const uint16_t SOFTBLINK;
extern const uint16_t DIM;
extern const uint16_t FADEOFF;
extern const uint16_t XENON;
extern const uint16_t BACKFIRE;
extern const byte ON;
extern const byte OFF;
extern const byte YES;
extern const byte NO;

extern const uint16_t Schemes[NumSchemes][NumLights][NumStates] PROGMEM;
