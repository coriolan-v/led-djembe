// Seed XIAO nrf52840

#include <bluefruit.h>


void setup(void) 
{
  Serial.begin(115200);

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
