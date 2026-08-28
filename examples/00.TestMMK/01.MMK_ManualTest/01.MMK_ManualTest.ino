/*
  MiniMicroKit_PTL - MMK_ManualTest - OLED (Rotary Encoder, Buzzer, LEDs, I2C)
  Developer: GelectronLAB
  Board: MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// ตัวแปรสำหรับระบบ Menu
int currentMenu = 0;
const int TOTAL_MENU = 4;

// ตัวแปร Timers สำหรับ Non-blocking Tasks
unsigned long uiRefreshTimer = 0;

// ตัวแปรควบคุมโหมด
int soundFxIndex = 0;
bool isBlinkingMode = false;

// ตัวแปรสำหรับหน้า 1 (ค่าเปอร์เซ็นต์เริ่มต้น 0 - 100%)
int testValueMode1 = 50;

void setup() {
  Serial.begin(115200);
  delay(1000); 
  Serial.println("\n=== [MiniMicroKit PT-L V1.0 Starting...] ===");

  if (!kit.begin(2, 1)) {
    Serial.println("[WARNING] OLED Display not found! Continuing hardware setup...");
  } else {
    Serial.println("[OK] OLED Display initialized successfully!");
  }

  kit.scanI2CBus(true);
  kit.setAutoSleep(true, 50000);

  if (kit.isOLEDPowered()) {
    kit.showDialog("MiniMicroKit PT-L", "System Ready!", 1200);
  }
}

void loop() {
  // Background Engine
  kit.update();

  // Rotary Encoder Processing: หมุนปรับค่าบาร์ได้ทันทีเฉพาะเมื่ออยู่หน้าแรก (currentMenu == 0)
  if (currentMenu == 0) {
    int stepValue = testValueMode1 / 5;
    stepValue = kit.updateMenuIndex(stepValue, 0, 20, false);
    testValueMode1 = stepValue * 5;
  }

  // Button Events Processing
  EncoderButtonEvent btnEvent = kit.getEncoderButton();

  if (btnEvent == ENC_BTN_CLICKED) {
    switch (currentMenu) {
      case 0: // คลิกสั้นไม่มีผล
        break;

      case 1: // Sound FX Engine (คลิกเพื่อเปลี่ยนเสียง)
        soundFxIndex = (soundFxIndex + 1) % 19;
        kit.playSystemSound((SoundType)soundFxIndex);
        break;

      case 2: // Async LEDs (คลิกเพื่อเปิด/ปิดไฟกระพริบ)
        isBlinkingMode = !isBlinkingMode;
        if (isBlinkingMode) {
          kit.blinkLED(true, true, true, 200);
          kit.playSystemSound(SOUND_SUCCESS);
        } else {
          kit.setLED(false, false, false);
          kit.beepAsync(80);
        }
        break;

      case 3: { // System Diagnostic (คลิกสั้นเพื่อสแกน I2C ค้างไว้ 3 วินาที)
        kit.playSystemSound(SOUND_CLICK);
        
        String foundStr = "No Device!";
        for (byte i = 1; i < 127; i++) {
          Wire.beginTransmission(i);
          if (Wire.endTransmission() == 0) {
            foundStr = "Found: 0x" + String(i, HEX);
            foundStr.toUpperCase();
            break;
          }
        }

        if (kit.isOLEDPowered()) {
          kit.showDialog("I2C SCANNER", foundStr, 3000);
        }
        break;
      }
    }
  } 
  else if (btnEvent == ENC_BTN_LONG_PRESS) {
    kit.playSystemSound(SOUND_SUCCESS);
    // เปลี่ยนหน้าด้วยการกดค้าง (Long Press) เท่านั้น
    currentMenu = (currentMenu + 1) % TOTAL_MENU;
  }

  // OLED UI Rendering (อัปเดตหน้าจอทุก 50ms)
  if (kit.every(uiRefreshTimer, 50)) {
    if (!kit.isOLEDPowered()) return; 

    kit.clearDisplay();

    // วาดเส้นแบ่งหน้าจอแนวตั้ง (ที่ตำแหน่ง X = 77)
    kit.display.drawLine(77, 9, 77, 63, SSD1306_WHITE);


    // ฝั่งซ้าย (60% พื้นที่: X = 0 ถึง 76)
    kit.printText("MiniMicro Test", 2, 0, 1);
    kit.display.drawLine(0, 9, 76, 9, SSD1306_WHITE);

    switch (currentMenu) {
      case 0: { // Bar & Value
        String valStr = String(testValueMode1) + "%";
        kit.display.setTextSize(2);
        int textWidth = valStr.length() * 12;
        int textX = (77 - textWidth) / 2;
        kit.display.setCursor(textX > 0 ? textX : 2, 16);
        kit.display.print(valStr);

        // บาร์แสดงผลกินพื้นที่ 60% ด้านล่างจากค่าตัวเลข
        kit.drawProgressBar(2, 35, 73, 10, testValueMode1);
        break;
      }

      case 1: { // Sound FX
        String soundNames[] = {      
          "CLICK", "SUCC", "ERROR", "START", "CONF",
          "CANCEL", "WARN", "ALARM", "NOTIFY", "LEVEL",
          "PWR", "BEEP", "DOUBLE", "HAPPY" , "CONNEC",
          "DISCON", "LASER", "COIN" , "HEART" 
          };
        String sndStr = soundNames[soundFxIndex];
        kit.display.setTextSize(2);
        int textWidth = sndStr.length() * 12;
        int textX = (77 - textWidth) / 2;
        kit.display.setCursor(textX > 0 ? textX : 0, 25);
        kit.display.print(sndStr);
        break;
      }

      case 2: { // LED & Beep
        String ledStr = isBlinkingMode ? "ON" : "OFF";
        kit.display.setTextSize(2);
        int textWidth = ledStr.length() * 12;
        int textX = (77 - textWidth) / 2;
        kit.display.setCursor(textX > 0 ? textX : 2, 25);
        kit.display.print(ledStr);
        break;
      }

      case 3: { // System Info
        kit.printText("AutoSleep", 10, 18, 1);
        kit.printText("15 Sec", 18, 28, 1);
        kit.printText("Scan I2C", 12, 38, 1);
        break;
      }
    }

    // ด้านล่างสุดของจอฝั่ง 60%: แสดงคำว่า "Press & hold" ขนาด 1
    kit.printText("Press & hold", 2, 53, 1);

    // ฝั่งขวา (40% พื้นที่: X = 78 ถึง 127)
    kit.printText(String(currentMenu + 1) + "/4", 98, 0, 1);
    kit.display.drawLine(78, 9, 127, 9, SSD1306_WHITE);

    kit.display.setTextSize(2);
    if (currentMenu == 0) {
      kit.display.setCursor(88, 22); kit.display.print("Bar");
      kit.display.setCursor(88, 42); kit.display.print("Adj");
    } else if (currentMenu == 1) {
      kit.display.setCursor(88, 22); kit.display.print("Buz");
      kit.display.setCursor(94, 42); kit.display.print("FX");
    } else if (currentMenu == 2) {
      kit.display.setCursor(89, 22); kit.display.print("LED");
      kit.display.setCursor(94, 42); kit.display.print("SW");
    } else if (currentMenu == 3) {
      kit.display.setCursor(88, 20); kit.display.print("I2C");
      kit.display.setCursor(80, 40); kit.display.print("Scan");
    }

    kit.displayUpdate();
  }
}