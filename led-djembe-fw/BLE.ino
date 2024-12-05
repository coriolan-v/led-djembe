
// OTA DFU service
BLEDfu bledfu;

// Uart over BLE service
BLEUart bleuart;

// Function prototypes for packetparser.cpp
uint8_t readPacket(BLEUart *ble_uart, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t *data, const uint32_t numBytes);

// Packet buffer
extern uint8_t packetbuffer[];

void initBLE() {
  Bluefruit.setName("LED Djembe");
  Bluefruit.begin();
  Bluefruit.setName("LED Djembe");
  Bluefruit.setTxPower(4);  // Check bluefruit.h for supported values

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and start the BLE Uart service
  bleuart.begin();

  // Set up and start advertising
  startAdv();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();
}

void startAdv(void) {
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include the BLE UART (AKA 'NUS') 128-bit UUID
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);  // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);    // number of seconds in fast mode
  Bluefruit.Advertising.start(0);              // 0 = Don't stop advertising after n seconds
}


void readBLE() {
  // Wait for new data to arrive
  uint8_t len = readPacket(&bleuart, 1);
  if (len == 0) return;

  // Got a packet!
  // printHex(packetbuffer, len);

  // Color
  if (packetbuffer[1] == 'C') {
    uint8_t red = packetbuffer[2];
    uint8_t green = packetbuffer[3];
    uint8_t blue = packetbuffer[4];
    Serial.print("RGB #");
    if (red < 0x10) Serial.print("0");
    Serial.print(red, HEX);
    if (green < 0x10) Serial.print("0");
    Serial.print(green, HEX);
    if (blue < 0x10) Serial.print("0");
    Serial.println(blue, HEX);

    Serial.println("Color!");

    //mode = 3;
    setMaxBrightness(255);

    setColor(red, green, blue);

  }

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print("Button ");
    Serial.print(buttnum);
    if (pressed) {
      Serial.println(" pressed");
    } else {
      Serial.println(" released");

      if (buttnum == 1) {
        mode = 0;  // normal mode
        Serial.println("MODE 0 ACTIVATED");
        setMaxBrightness(0);
      }
      if (buttnum == 2) {
        mode = 1;  // demo mode, no interaction
         setMaxBrightness(255);
      }
      if (buttnum == 3) {
        mode = 3;  // fixed color, no interaction (set using color picker)
         setMaxBrightness(255);
      }
      if (buttnum == 4) {
        mode = 4;  // normal mode but with fixed color (set using color picker)
         ///setMaxBrightness(255);
        Serial.println("MODE 4 ACTIVATED");
        setMaxBrightness(0);
       // setColor(red, green, blue);
      }

      // if (buttnum == 2) ledStatus = 2; // oFF
      // if (buttnum == 3) ledStatus = 3; // CONTINUOUS
    }
  }
}
