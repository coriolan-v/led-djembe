
#include <bluefruit.h>

#include "RunningAverage.h"


RunningAverage myRA(10);
int samples = 0;


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

    myRA.clear();  //  explicitly start clean
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  int sensorValue = analogRead(A2);
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
  delay(15);  // delay in between reads for stability
}
