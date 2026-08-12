/**
 * This sketch is specifically for programming the EEPROM used in the 8-bit
 * decimal display decoder described in https://youtu.be/dLh1n2dErzE
 * Ported and optimized for ESP32.
 */
#define SHIFT_DATA  21
#define SHIFT_CLK   22
#define SHIFT_LATCH 23
#define WRITE_EN    19

// Safe, non-sequential ESP32 pins mapping to EEPROM Data lines D0 through D7
const int EEPROM_PINS[8] = {13, 14, 25, 26, 27, 32, 33, 4}; 

/*
   Output the address bits and outputEnable signal using shift registers.
*/
void setAddress(int address, bool outputEnable) {
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);

  digitalWrite(SHIFT_LATCH, LOW);
  delayMicroseconds(1);
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
   Write a byte to the EEPROM at the specified address.
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
  delayMicroseconds(2); // Meet AT28C minimum WE pulse-width requirement
  digitalWrite(WRITE_EN, HIGH);
  delay(10);            // Internal byte-write cycle timing buffer
}


/*
   Read the contents of the EEPROM and print them to the serial monitor.
*/
void printContents() {
  // Read first 2048 bytes to capture all programmed display partitions
  for (int base = 0; base < 2048; base += 16) {
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


void setup() {
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);
  
  Serial.begin(115200);
  delay(500);
  
  // Bit patterns for the digits 0..9
  byte digits[] = { 0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b };

  Serial.println("Programming ones place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value, digits[value % 10]);
  }
  Serial.println("Programming tens place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 256, digits[(value / 10) % 10]);
  }
  Serial.println("Programming hundreds place");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 512, digits[(value / 100) % 10]);
  }
  Serial.println("Programming sign");
  for (int value = 0; value <= 255; value += 1) {
    writeEEPROM(value + 768, 0);
  }

  Serial.println("Programming ones place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1024, digits[abs(value) % 10]);
  }
  Serial.println("Programming tens place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1280, digits[abs(value / 10) % 10]);
  }
  Serial.println("Programming hundreds place (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    writeEEPROM((byte)value + 1536, digits[abs(value / 100) % 10]);
  }
  Serial.println("Programming sign (twos complement)");
  for (int value = -128; value <= 127; value += 1) {
    if (value < 0) {
      writeEEPROM((byte)value + 1792, 0x01);
    } else {
      writeEEPROM((byte)value + 1792, 0);
    }
  }

  // Read and print out the contents of the EEPROM
  Serial.println("Reading EEPROM");
  printContents();
}


void loop() {
  // Unused
}
