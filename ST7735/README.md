# ESP32 + ST7735 TFT Display (Text Demo)

Simple ESP32 project that initializes an ST7735 TFT screen and displays basic text. The screen also flashes inverted colors to indicate the system is running.

---

## 📌 Features
- ESP32 driven TFT display (ST7735)
- Shared SPI setup with SD card support (disabled during display test)
- Basic text rendering
- System heartbeat animation (screen invert blink)

---

## 🔧 Hardware Used

- ESP32 Development Board
- ST7735 TFT Display (128x160)
- Optional SD Card Module (SPI shared bus)

---

## 🔌 Wiring (ST7735 TFT)

| TFT Pin | ESP32 Pin |
|--------|-----------|
| VCC    | 3.3V      |
| GND    | GND       |
| SCL    | GPIO 18 (SCLK) |
| SDA    | GPIO 23 (MOSI) |
| RES    | GPIO 4    |
| DC     | GPIO 2    |
| CS     | GPIO 17   |
| LED    | 3.3V      |

---

## ⚙️ SD Card (Optional)

| SD Pin | ESP32 Pin |
|--------|-----------|
| CS     | GPIO 5    |
| MOSI   | GPIO 23   |
| MISO   | GPIO 19   |
| SCK    | GPIO 18   |

> Note: SD card is disabled in this demo to prevent SPI conflicts during TFT initialization.

---

## 🧠 How It Works

1. Initializes Serial for debugging
2. Disables SD card chip select to avoid SPI conflicts
3. Initializes ST7735 display in `INITR_BLACKTAB` mode
4. Clears screen and sets rotation to landscape
5. Prints two text lines:
   - "ISSAC ENGINEER"
   - "SYSTEM OK"
6. Continuously inverts display every 500ms as a heartbeat indicator

---

## 📦 Libraries Required

Install via Arduino Library Manager:

- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library
- SPI (built-in)

---

## 🚀 Upload Settings

- Board: ESP32 Dev Module
- Flash Mode: QIO
- Upload Speed: 115200 or higher
- Partition Scheme: Default

---

## 📷 Output

- Green header text: `TEST ENGINEER`
- White status text: `SYSTEM OK`
- Screen flashes every 0.5 seconds

---

## 🛠 Notes

- If colors look wrong, try:
  - `INITR_GREENTAB`
  - `INITR_REDTAB`
- Ensure all devices share common GND
- Keep SPI wiring short for stability

---

## 📁 Project Status
✔ Working basic TFT display test  
✔ SPI shared bus configured  
✔ SD card safely isolated during TFT use  
