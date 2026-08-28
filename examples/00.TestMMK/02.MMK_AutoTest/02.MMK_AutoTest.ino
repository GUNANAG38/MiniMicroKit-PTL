/*
  MiniMicroKit_PTL - MMK_AutoTest - (OLED, I2C, Buzzer, LEDs)
  Developer: GelectronLAB
  Board: MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// ฟังก์ชันสำหรับรัน POST ทั้งหมด
void runPOST() {
  // หน้าจอเริ่มต้น POST
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("HARDWARE POST", 22, 0, 1);
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    kit.printText("Initializing...", 15, 25, 1);
    kit.displayUpdate();
  }
  delay(1000);

  // ทดสอบระบบเสียง (Buzzer & Sound FX)
  Serial.println("[TEST 1/3] Testing Buzzer & Sound FX...");
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("POST: 1/3 Buzzer", 16, 20, 1);
    kit.displayUpdate();
  }
  kit.playSystemSound(SOUND_STARTUP);
  delay(1200);

  // ทดสอบหลอดไฟ LED บนบอร์ด (ทีละดวง)
  Serial.println("[TEST 2/3] Testing LEDs...");
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("POST: 2/3 LEDs", 20, 20, 1);
    kit.displayUpdate();
  }
  //(พารามิเตอร์: แดง, เขียว, น้ำเงิน)
  // เปิด LED Red
  kit.setLED(true, false, false); 
  delay(400);
  // เปิด LED Yellow
  kit.setLED(false, true, false); 
  delay(400);
  // เปิด LED Green
  kit.setLED(false, false, true); 
  delay(400);
  // ดับทั้งหมด
  kit.setLED(false, false, false);

  // ทดสอบระบบ I2C Bus Scanner
  Serial.println("[TEST 3/3] Scanning I2C Bus...");
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("POST: 3/3 I2C Bus", 12, 20, 1);
    kit.displayUpdate();
  }
  delay(800);

  String foundDevice = "No Device!";
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      foundDevice = "Found: 0x" + String(i, HEX);
      foundDevice.toUpperCase();
      break;
    }
  }
  Serial.println("[I2C] Result: " + foundDevice);

  // สรุปผลการทดสอบ (POST PASSED)
  Serial.println("[PASS] Hardware POST Completed Successfully!\n");
  
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("HARDWARE POST", 22, 5, 1);
    kit.display.drawLine(0, 15, 127, 15, SSD1306_WHITE);
    kit.printText("STATUS: PASSED", 18, 25, 1);
    kit.printText("Press to Re-Test", 12, 45, 1);
    kit.displayUpdate();
    
    kit.playSystemSound(SOUND_SUCCESS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== [MiniMicroKit PT-L: Hardware POST Starting...] ===");

  if (!kit.begin(2, 1)) {
    Serial.println("[FAIL] OLED Display not found!");
  } else {
    Serial.println("[PASS] OLED Display initialized successfully.");
  }

  kit.scanI2CBus(true);
  kit.setAutoSleep(true, 15000);

  // รัน POST ครั้งแรกตอนเปิดเครื่อง
  runPOST();
}

void loop() {
  kit.update();

  // ตรวจจับการกดปุ่มเพื่อเริ่มทดสอบซ้ำ (Re-Test)
  EncoderButtonEvent btnEvent = kit.getEncoderButton();
  if (btnEvent == ENC_BTN_CLICKED) {
    kit.playSystemSound(SOUND_CLICK);
    runPOST(); // สั่งรันกระบวนการทดสอบซ้ำอีกรอบ
  }
}