#include <Arduino.h>

void PrintSpaceDash()
{
    Serial.print(F(" - "));
}

void PrintHorizontalLine()
{
    Serial.println(F("-----------------------------------"));
}

void PrintTrueFalse(boolean boolVal)
{
    if (boolVal == true) { Serial.println(F("TRUE")); } else { Serial.println(F("FALSE")); }
}
