/*
 * =========================================================================
 * HARDWARE: ESP32 + ST7735 + SD Card (Shared SPI)
 * =========================================================================
 * ST7735 TFT SCREEN:
 * - VCC    -> 3.3V
 * - GND    -> GND
 * - SCL    -> GPIO 18 (Shared SCLK)
 * - SDA    -> GPIO 23 (Shared MOSI)
 * - RES    -> GPIO 4  (Reset)
 * - DC/A0  -> GPIO 2  (Data/Command)
 * - CS     -> GPIO 17 (TFT Chip Select)
 * - LED    -> 3.3V    (Backlight)
 *
*/

#include <Adafruit_GFX.h>    // The "Drawing" instructions (Lines, Circles, Text)
#include <Adafruit_ST7735.h> // The "Driver" for your specific 7735 chip
#include <SPI.h>             // The "Language" used to talk to the SD and Screen

#define TFT_DC   2 
#define TFT_CS   17
#define TFT_RST  4
#define SD_CS    5

// Initialize Adafruit ST7735
// Note: MOSI (23) and SCLK (18) are handled by the SPI library automatically
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  delay(1500);
  Serial.begin(115200);

  // CRITICAL: Disable SD Card so it doesn't interfere with the screen test
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  // Initialize the screen (use INITR_BLACKTAB or INITR_REDTAB if colors look weird)
  tft.initR(INITR_BLACKTAB); 
  
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(3); // Landscape mode
  
  // Draw some test text
  tft.setCursor(10, 30);
  tft.setTextColor(ST7735_GREEN);
  tft.setTextSize(1);
  tft.println("TEST ENGINEER");
  
  tft.setCursor(10, 50);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("SYSTEM OK");

  Serial.println("Display Initialized.");
}

void loop() {
  // Flash the "System OK" to show the ESP32 is alive
  tft.invertDisplay(true);
  delay(500);
  tft.invertDisplay(false);
  delay(500);
}
