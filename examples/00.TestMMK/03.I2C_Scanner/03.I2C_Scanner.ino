/*
  MiniMicroKit_PTL - I2C Bus Scanner Tool
  Developer: GelectronLAB
  Board: MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <Wire.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

unsigned long uiTimer = 0;
String scanResultText = "Scanning...";
int deviceCount = 0;

// ฟังก์ชันสแกนบัส I2C
void performScan() {
  Serial.println("\n--- Scanning I2C Bus (SDA:2, SCL:1) ---");
  deviceCount = 0;
  String foundList = "";

  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      String hexAddr = "0x" + String(i, HEX);
      hexAddr.toUpperCase();
      Serial.print("Device found at ");
      Serial.println(hexAddr);
      
      if (foundList == "") {
        foundList = hexAddr;
      } else {
        foundList += ", " + hexAddr;
      }
      deviceCount++;
    }
  }

  if (deviceCount == 0) {
    Serial.println("No I2C devices found!");
    scanResultText = "No Device Found!";
  } else {
    Serial.print("Total found: ");
    Serial.println(deviceCount);
    scanResultText = foundList;
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== [MiniMicroKit PT-L: I2C Scanner] ===");

  // เริ่มต้นระบบ OLED (SDA = 2, SCL = 1)
  if (!kit.begin(2, 1)) {
    Serial.println("[WARNING] OLED Display not found!");
  }

  // ตั้งค่า Auto-Sleep ดับจอหากไม่ใช้งาน 15 วินาที
  kit.setAutoSleep(true, 15000);

  if (kit.isOLEDPowered()) {
    kit.showDialog("I2C SCANNER", "Ready to Scan", 1000);
  }

  // สแกนครั้งแรกเมื่อเริ่มต้นระบบ
  performScan();
}

void loop() {
  // อัปเดตสถานะ
  kit.update();

  // กดปุ่ม Encoder (SW) เพื่อสั่งสแกนซ้ำ
  EncoderButtonEvent btn = kit.getEncoderButton();
  if (btn == ENC_BTN_CLICKED) {
    kit.playSystemSound(SOUND_CLICK);
    performScan();
    if (kit.isOLEDPowered()) {
      kit.showDialog("SCAN COMPLETE", "Found: " + String(deviceCount) + " devices", 1200);
    }
  }

  // ส่วนแสดงผลหน้าจอ OLED (อัปเดตทุก 100ms)
  if (!kit.isOLEDPowered()) return;

  if (kit.every(uiTimer, 100)) {
    kit.clearDisplay();
    
    kit.printText("I2C SCANNER", 30, 0, 1);
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    kit.printText("SDA: GPIO 2", 0, 16, 1);
    kit.printText("SCL: GPIO 1", 0, 28, 1);
    
    kit.printText("Found:", 0, 40, 1);
    kit.printText(scanResultText, 42, 40, 1);

    kit.printText("[Click to Re-Scan]", 12, 54, 1);

    kit.displayUpdate();
  }
}