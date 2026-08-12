#define SHIFT_DATA  21
#define SHIFT_CLK   22
#define SHIFT_LATCH 23
#define WRITE_EN    19

// Array of safe, non-sequential ESP32 GPIOs for the 8 data lines
const int EEPROM_PINS[8] = {13, 14, 25, 26, 27, 32, 33, 4}; // D0 to D7
/*
 * Output the address bits and outputEnable signal using shift registers.
 */
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  delayMicroseconds(1); // Give shift register time to catch up to ESP32 speed
  digitalWrite(SHIFT_LATCH, HIGH);
  delayMicroseconds(1);
  digitalWrite(SHIFT_LATCH, LOW);
}


/*
 * Read a byte from the EEPROM at the specified address.
 */
 byte readEEPROM(int address) {
  // Set ESP32 data lines as inputs
  for (int i = 0; i < 8; i++) {
    pinMode(EEPROM_PINS[i], INPUT);
  }

  // Set address AND assert Output Enable (OE = LOW)
  setAddress(address, /*outputEnable*/ true);
  delayMicroseconds(5); // Allow EEPROM access time

  byte data = 0;
  // Read D7 first, D0 last — same shift-and-add style as Ben's original
  for (int i = 7; i >= 0; i--) {
    data = (data << 1) + digitalRead(EEPROM_PINS[i]);
  }

  return data;
}

/*
 * Write a byte to the EEPROM at the specified address.
 */
void writeEEPROM(int address, byte data) {
  setAddress(address, /*outputEnable*/ false);
  
  for (int i = 0; i < 8; i++) {
    pinMode(EEPROM_PINS[i], OUTPUT);
    digitalWrite(EEPROM_PINS[i], data & 1);
    data = data >> 1;
  }
  
  delayMicroseconds(1); 
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(2);  // EEPROM requires WE pulse width ~1-2 microseconds
  digitalWrite(WRITE_EN, HIGH);
  delay(10);             // EEPROM internal write cycle cycle time
}


/*
 * Read the contents of the EEPROM and print them to the serial monitor.
 */
void printContents() {
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readEEPROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}


// 4-bit hex decoder for common anode 7-segment display
byte data[] = { 0x81, 0xcf, 0x92, 0x86, 0xcc, 0xa4, 0xa0, 0x8f, 0x80, 0x84, 0x88, 0xe0, 0xb1, 0xc2, 0xb0, 0xb8 };

// 4-bit hex decoder for common cathode 7-segment display
// byte data[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b, 0x77, 0x1f, 0x4e, 0x3d, 0x4f, 0x47 };


void setup() {
  // put your setup code here, to run once:
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  Serial.begin(57600);

  // Erase entire EEPROM
  Serial.print("Erasing EEPROM");
  for (int address = 0; address <= 2047; address += 1) {
    writeEEPROM(address, 0xff);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");


  // Program data bytes
  Serial.print("Programming EEPROM");
  for (int address = 0; address < sizeof(data); address += 1) {
    writeEEPROM(address, data[address]);

    if (address % 64 == 0) {
      Serial.print(".");
    }
  }
  Serial.println(" done");


  // Read and print out the contents of the EERPROM
  Serial.println("Reading EEPROM");
  printContents();
}


void loop() {
  // put your main code here, to run repeatedly:

}
