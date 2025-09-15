# RFID Attendance System with ESP8266 + Google Sheets Integration

This project is an **IoT-based RFID Attendance System** using **ESP8266 (or ESP32)** and **MFRC522 RFID Reader**.  
It integrates with **Google Sheets via Google Apps Script**, allowing real-time attendance marking and daily summary reports.

---

## 🚀 Features
- 📡 Connects ESP8266/ESP32 to Wi-Fi and sends data securely using HTTPS.  
- 🪪 Reads RFID cards (UIDs) with MFRC522 module.  
- 📝 Marks **Time In** and **Time Out** automatically in Google Sheets.  
- 🔄 Supports **multiple IN/OUT per day** (e.g., breaks, lunch).  
- 🚫 Rejects unauthorized cards (UIDs not in student database).  
- 📊 Generates **daily summary at 4:30 PM** (attendance %, absentees, extended breaks).  
- ⚡ Batch mode for updating multiple UIDs at once.  

---

## 🛠️ Hardware Requirements
- **ESP8266 (NodeMCU) or ESP32 board**  
- **MFRC522 RFID Reader**  
- **RFID Cards/Tags**  
- **Jumper wires & breadboard**  

**Connections (ESP8266 example):**
| MFRC522 Pin | ESP8266 Pin |
|-------------|-------------|
| SDA (SS)    | D2          |
| RST         | D1          |
| MOSI        | D7          |
| MISO        | D6          |
| SCK         | D5          |
| 3.3V        | 3.3V        |
| GND         | GND         |
-----------------------------

> ⚠️ MFRC522 works on **3.3V logic only**. Don’t use 5V directly.

---

## 🧑‍💻 Software Setup

### 1. ESP8266/ESP32 Firmware
- Install Arduino IDE and add ESP8266/ESP32 board support.  
- Install required libraries:
  - `ESP8266WiFi` (or `WiFi.h` for ESP32)
  - `WiFiClientSecure`
  - `SPI`
  - `MFRC522`  
- Update Wi-Fi credentials in the code:
  ```cpp
  const char* ssid = "YourWiFiName";
  const char* password = "YourWiFiPassword";
