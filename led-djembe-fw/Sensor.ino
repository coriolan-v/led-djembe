#include "RunningAverage.h"


RunningAverage myRA(10);
int samples = 0;

unsigned long prevMill_readSensor = 0;
int intervalReadSensor = 25;
int sensorValue = 0;

int sensorTresholdDifference = 90;

void initSensor() {
  myRA.clear();  //  explicitly start clean
}

void runSensor() {
  if (millis() - prevMill_readSensor >= intervalReadSensor) {
    prevMill_readSensor = millis();

    sensorValue = analogRead(A0);

    myRA.addValue(sensorValue);
    // print out the value you read:
    Serial.println();
    Serial.print(sensorValue);
    Serial.print("\t");
    Serial.print(myRA.getAverage(), 3);
    Serial.print("\t");
    Serial.print(myRA.getAverageSubset(0, 5), 3);
    Serial.print("\t");
    Serial.print(myRA.getAverageSubset(3, 5), 3);
    Serial.print("\t");
    Serial.println(myRA.getAverageSubset(5, 5), 3);
    Serial.println();

    if (myRA.getAverage() >= sensorTresholdDifference) {
      Serial.println("BONG");

      triggerLEDflag = true;
      stampMill_triggerLED = millis();

      currentBrightness = MAXBRIGHTNESS;
      setMaxBrightness(MAXBRIGHTNESS);
    }
  }
}