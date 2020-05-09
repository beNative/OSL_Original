#ifndef LED_H
#define LED_H
  #include <Arduino.h>

  void RedLedOn();
  void RedLedOff();
  void GreenLedOn();
  void GreenLedOff();
  void GreenBlink();
  void RedBlink();
  void ToggleAllLights();
  void GreenBlinkSlow(int HowMany);
  void RedBlinkFast(int HowMany);
  void GreenBlinkFast(int HowMany);
  // If these seem redundant it is because SimpleTimer can only call a void function
  void GreenBlinkOne();
  void GreenBlinkTwo();
  void GreenBlinkThree();
  void RedBlinkOne();
  void RedBlinkTwo();
  void RedBlinkThree();
#endif // ndef LED_H
