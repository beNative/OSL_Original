#ifndef RX_COMMANDS_H
#define RX_COMMANDS_H

  #include <Arduino.h>
  void GetRxCommands();
  bool CheckChannel3();
  bool CheckSteeringChannel();
  int GetThrottleCommand();
  int GetTurnCommand();
  int GetChannel3Command();
  int smoothThrottleCommand(int rawVal);
  int smoothSteeringCommand(int rawVal);
  int smoothChannel3Command(int rawVal);

  void RadioSetup();

#endif // ndef RX_COMMANDS_H
