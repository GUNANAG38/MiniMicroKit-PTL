
/*
  MiniMicroKit_PTL - Comprehensive Rotary Encoder & Button Example
  
  Demonstrates all Encoder and Button control functions available in the library:
  - Rotational step tracking with range limits & wrapping (kit.updateMenuIndex)
  - Short click button event detection (ENC_BTN_CLICKED)
  - Long press button event detection (ENC_BTN_LONG_PRESS)
  - Auto OLED wake-up on encoder activity
  
  Developer: GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// ตัวแปรสำหรับเก็บค่าตำแหน่งการหมุน
int counterValue = 0;
const int MIN_VAL = 0;
const int MAX_VAL = 10;

// ตัวแปร Timers สำหรับอัปเดตจอ OLED
unsigned long uiRefreshTimer = 0;
String lastEventStr = "None";

void setup() {
  Serial.begin(115200);
  delay(1000);

  kit.begin();
  Serial.println("[INFO] MiniMicroKit_PTL Encoder Basic Demo Started");

  // ตั้งเวลา Auto-Sleep ดับจอหากไม่ใช้งานเกิน 10 วินาที
  kit.setAutoSleep(true, 10000);

  // แสดง Pop-up ต้อนรับ
  if (kit.isOLEDPowered()) {
    kit.showDialog("ENCODER DEMO", "Rotate & Press SW", 1200);
  }
}

void loop() {
  // เรียก kit.update() ใน loop() เสมอ
  // เพื่อประมวลผลการอ่านค่า Debounce ของปุ่มกดและระบบ Auto-Sleep
  kit.update();

  // อ่านการหมุนลูกบิด Encoder (พร้อมจำกัดช่วงและหมุนวนรอบ)
  // พารามิเตอร์ : updateMenuIndex(currentVal, minVal, maxVal, wrap_bool)
  int newCounter = kit.updateMenuIndex(counterValue, MIN_VAL, MAX_VAL, true);

  // ตรวจจับเมื่อมีการหมุนเปลี่ยนค่า
  if (newCounter != counterValue) {
    counterValue = newCounter;
    Serial.printf("[ENCODER] Current Value: %d\n", counterValue);
    
    // ส่งเสียง Beep สั้นๆ ตอบรับการหมุน
    kit.beepAsync(30); 
  }

  // อ่านสถานะการกดปุ่ม Encoder SW (GPIO0)
  // การเรียกใช้ฟังก์ชันนี้จะปลุกจอ OLED ให้ติดกลับขึ้นมาอัตโนมัติหากจอดับอยู่
  EncoderButtonEvent btnEvent = kit.getEncoderButton();

  if (btnEvent == ENC_BTN_CLICKED) {
    // กดสั้น (Click)
    lastEventStr = "CLICKED";
    Serial.println("[BUTTON] Short Click Detected!");
    
    // เล่นเสียงยืนยัน
    kit.playSystemSound(SOUND_CLICK);
    
  } else if (btnEvent == ENC_BTN_LONG_PRESS) {
    // กดค้างเกิน 800ms (Long Press)
    lastEventStr = "LONG PRESS";
    Serial.println("[BUTTON] Long Press Detected!");
    
    // เล่นเสียงเอฟเฟกต์สำเร็จ
    kit.playSystemSound(SOUND_SUCCESS);
    
    // รีเซ็ตค่าตัวนับกลับเป็น 0 เมื่อกดค้าง
    counterValue = 0;
  }

  // วาดค่าปัจจุบันขึ้นจอ OLED
  if (!kit.isOLEDPowered()) return; // หากจอดับอยู่ ให้ข้ามลูปการวาดภาพ

  if (kit.every(uiRefreshTimer, 50)) {
    kit.clearDisplay();

    // Header
    kit.printText("ENCODER TEST", 0, 0, 1);
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    // Body
    kit.printText("Value : " + String(counterValue) + " [" + String(MIN_VAL) + "-" + String(MAX_VAL) + "]", 0, 16, 1);
    kit.printText("Button: " + lastEventStr, 0, 28, 1);
    kit.printText("(Long Press to Reset)", 0, 40, 1);

    // Bar แสดงสัดส่วนตามค่าที่หมุนได้
    int percent = map(counterValue, MIN_VAL, MAX_VAL, 0, 100);
    kit.drawProgressBar(0, 52, 128, 10, percent);

    kit.displayUpdate();
  }
}