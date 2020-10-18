#include <Arduino.h>
#include <avr/pgmspace.h>

extern const int NA;
extern const int BLINK;
extern const int FASTBLINK;
extern const int SOFTBLINK;
extern const int DIM;
extern const int FADEOFF;
extern const int XENON;
extern const int BACKFIRE;

extern const byte ON;
extern const byte OFF;

extern const byte YES;
extern const byte NO;

extern const int Schemes[NumSchemes][NumLights][NumStates] PROGMEM;
