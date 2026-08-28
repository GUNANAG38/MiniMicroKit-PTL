/*
  MiniMicroKit_PTL - Comprehensive OLED Display Control Example
  
  Demonstrates all OLED display functions available in the library:
  - Display Initialization & Power Status Check (kit.isOLEDPowered)
  - Auto-Sleep configuration (kit.setAutoSleep)
  - Pop-up Dialogs (kit.showDialog)
  - Text Printing with font scaling (kit.printText)
  - Progress Bar Rendering (kit.drawProgressBar)
  - Primitive Graphic Shapes via kit.display (Lines, Rectangles, Circles)
  - Frame Clear & Update cycle (kit.clearDisplay & kit.displayUpdate)
  
  Developer: GelectronLAB
  Board: MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// ตัวแปร Timers สำหรับทำ Non-blocking
unsigned long uiRefreshTimer = 0;
int progressVal = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);


  // เริ่มต้นระบบจอ OLED (Default SDA = GPIO2, SCL = GPIO1)
  if (!kit.begin()) {
    Serial.println("[ERROR] OLED Display not found!");
  } else {
    Serial.println("[INFO] OLED Display Initialized Successfully");
  }

  // ตั้งเวลาดับจออัตโนมัติเมื่อไม่มีการใช้งาน (Auto-Sleep)
  // พารามิเตอร์: setAutoSleep(enable_bool, timeout_ms)
  kit.setAutoSleep(true, 15000); // เปิด Auto-Sleep ดับจอหลังจากไม่กด/หมุนเกิน 15 วินาที

  // แสดง Pop-up Dialog แจ้งเตือนตรงกลางจอแบบกำหนดเวลา
  // พารามิเตอร์: showDialog(Title, Message, displayTime_ms)
  if (kit.isOLEDPowered()) {
    kit.showDialog("GELECTRON LAB", "OLED Test", 1500);
  }
}

void loop() {
  // ต้องเรียก kit.update() ใน loop() เสมอ
  // เพื่อประมวลผลระบบนับเวลา Auto-Sleep ของหน้าจอ
  kit.update();

  // อ่านปุ่มกด Encoder (เมื่อกดปุ่ม จอจะกลับมาติดทันที!)
  EncoderButtonEvent btnEvent = kit.getEncoderButton();
  if (btnEvent == ENC_BTN_CLICKED) {
    kit.beepAsync(50); // ส่งเสียงตอบรับการกดปุ่ม
  }
  // เช็กสถานะจอภาพก่อนทำการวาดเฟรมใหม่
  // หากจอดับอยู่ (Auto-Sleep) ให้ข้ามการประมวลผลเพื่อประหยัด CPU
  if (!kit.isOLEDPowered()) return;

  // วาดและอัปเดตหน้าจอทุกๆ 50ms (20 FPS) แบบ Non-blocking
  if (kit.every(uiRefreshTimer, 50)) {

    // เคลียร์บัฟเฟอร์เฟรมเก่าออกก่อนเริ่มวาดใหม่
    kit.clearDisplay();

    // พิมพ์ข้อความบนจอ OLED
    // พารามิเตอร์ : printText(text_string, x, y, font_size)
    kit.printText("OLED Test", 0, 0, 1);                     // ตัวอักษรขนาดปกติ (size 1)
    kit.printText(String(progressVal) + "%", 100, 0, 1);

    // พิมพ์ข้อความขนาดใหญ่
    // kit.printText("BIG", 40, 20, 2);                      // ตัวอักษรขนาดใหญ่ (size 2)

    // วาดเส้นตรงและรูปทรงเรขาคณิต (ผ่านวัตถุ kit.display)
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);     // เส้นกั้นแนวนอนใต้ Header

    // ตัวอย่างวาดรูปทรงอื่นๆ
    // kit.display.drawRect(0, 15, 60, 20, SSD1306_WHITE);   // วาดกรอบสี่เหลี่ยม (x, y, w, h, color)
    // kit.display.fillRect(65, 15, 60, 20, SSD1306_WHITE);  // วาดสี่เหลี่ยมแบบทึบ
    // kit.display.drawCircle(30, 25, 8, SSD1306_WHITE);     // วาดวงกลม (x, y, radius, color)

    // วาดหลอดProgress Bar พร้อมคำนวณ % ถมสีอัตโนมัติ
    // พารามิเตอร์ : drawProgressBar(x, y, width, height, percentage)
    kit.printText("Status: Downloading..", 0, 16, 1);
    kit.printText("Value : " + String(progressVal) + " / 100", 0, 28, 1);
    kit.drawProgressBar(0, 42, 128, 14, progressVal);

    // สั่งส่งข้อมูลบัฟเฟอร์ภาพขึ้นไปแสดงผลบนหน้าจอจริง
    kit.displayUpdate();

    // จำลองการเพิ่มค่า Progress (0 -> 100%)
    progressVal += 2;
    if (progressVal > 100) {
      progressVal = 0;
    }
  }
}