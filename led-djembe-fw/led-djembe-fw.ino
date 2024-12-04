// Feather nrf52840 Express

#include <bluefruit.h>


void setup(void) 
{
  Serial.begin(115200);

  while (!Serial)  {};

  Serial.println(F("LED DJUMBE"));
  Serial.println(F("-------------------------------------------"));

  initLED();

  initBLE();

  initSensor();
}



void loop(void) 
{
  runSensor();

  runLED();

  readBLE();
}
