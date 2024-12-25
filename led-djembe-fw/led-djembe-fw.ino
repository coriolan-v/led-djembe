// Feather nrf52840 Express

#include <bluefruit.h>

int mode = 0;


void setup(void) 
{
  Serial.begin(115200);

 // while (!Serial)  {};

  Serial.println(F("LED DJUMBE"));
  Serial.println(F("-------------------------------------------"));

  initLED();

  initBLE();

  initSensor();
}



void loop(void) 
{
  runSensor();
  //delay(10);

  runLED();

  readBLE();
}
