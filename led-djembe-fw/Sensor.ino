unsigned long prevMill_readSensor = 0;
int intervalReadSensor = 25;
int sensorValue = 0;

const int sensorPin = A0;    // Analog input pin for the sensor
const int numReadings = 10;  // Number of readings for the moving average

int readings[numReadings];  // Array to store the readings
int readIndex = 0;          // Index of the current reading
int total = 0;              // Sum of the readings
int average = 0;            // Average of the readings
int previousAverage = 0;    // Previous average for comparison

const int minTreshold = 10;  // Relative threshold value
const int maxTreshold = 90;  // Relative threshold value
int lastPixels = 0;
int lastPixels_R = 58;
int lastPixels_L = 58;

void initSensor() {
  pinMode(sensorPin, INPUT);
}



void runSensor() {

  if (mode == 0) {
    if (millis() - prevMill_readSensor >= intervalReadSensor) {
      prevMill_readSensor = millis();

      total = total - readings[readIndex];          // Subtract the last reading
      readings[readIndex] = analogRead(sensorPin);  // Read the sensor
      total = total + readings[readIndex];          // Add the new reading
      readIndex = (readIndex + 1) % numReadings;    // Advance to the next position

      previousAverage = average;      // Store the previous average
      average = total / numReadings;  // Calculate the new average

      //Serial.print("Sensor value: ");
      Serial.print(readings[readIndex]);
      Serial.print(",");
      Serial.print(average);
      Serial.print(",");
      int absAv = abs(average - previousAverage);
      Serial.println(absAv);

      if (absAv > minTreshold) {

        triggerLEDflag = true;
        stampMill_triggerLED = millis();

        currentBrightness = map(absAv, minTreshold, maxTreshold, 50, MAXBRIGHTNESS);
        motionSpeed = map(absAv, minTreshold, maxTreshold, 1, 5);
        maxPosLED = map(absAv, minTreshold, maxTreshold, 100, 0);

        maxRangePC = map(absAv, minTreshold, maxTreshold, 0, 100);
        firstPixel = map(maxRangePC, 0, 100, 0, NUM_LEDS);
        lastPixels = NUM_LEDS;
        lastPixel = map(maxRangePC, 0, 100, NUM_LEDS, 59);

        Serial.print("++++ BONG ");
        Serial.print(absAv);
        Serial.print(",");
        Serial.print(maxRangePC);
        Serial.print(",");
        Serial.print(firstPixel);
        Serial.print(",");
        Serial.print(lastPixel);
        Serial.println();
        setMaxBrightness(currentBrightness);



        if (maxRangePC > 70) {
          chooseRandomPalette();
          Serial.println("NEW PALETTE");
        }

        if (maxRangePC > 50) {
          gReverseDirection = !gReverseDirection;
        }
      }

      if (triggerLEDflag == true && (millis() - stampMill_triggerLED >= LEDdecayMs)) {
        // FillLEDsFromPaletteColors(startIndex);
        currentBrightness = currentBrightness - decreaseIncrement;
        constrain(currentBrightness, 0, 255);
        if (currentBrightness < 0) currentBrightness = 0;
        setMaxBrightness(currentBrightness);
        Serial.print("____BRI: ");
        Serial.println(currentBrightness);
        if (currentBrightness < 1) triggerLEDflag = false;
      }
    }
  }
}