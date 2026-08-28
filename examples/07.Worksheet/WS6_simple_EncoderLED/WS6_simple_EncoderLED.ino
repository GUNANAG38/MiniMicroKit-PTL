/*
  Worksheet simple
  LAB 6 : WS_simple_EncoderLED
  โปรแกรม : ควบคุมไฟ LED ด้วย Rotary Encoder แสดงผลการควบคุมด้วย LED และจอ OLED 
  โจทย์เงื่อนไข :  ใช้ Rotary Encoder เป็นตัวหมุนเลือกโหมดหรือระดับการทำงาน แสดงผลหมายเลขโหมดบนจอ OLED (ช่วง 0–3) 
                และตั้งเงื่อนไข เมื่อหมุนเปลี่ยนโหมด ให้สั่งเปลี่ยนสถานะหรือสีของไฟ LED ตามโหมดที่เลือก
  คำแนะนำ : ให้นักเรียนเติมเต็มตรรกะในบล็อกเงื่อนไข if-else (หรือ switch-case) 
            เพื่อควบคุมการทำงานของ LED ให้สอดคล้องกับค่า ledMode ที่หมุนเลือก
*/

#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;
int lastEncoderVal = 0; // ค่าเริ่มต้นของโหมด LED (0 ถึง 3)

void setup() {
  kit.begin();
}

void loop() {
  kit.update();
  
  // ใช้ Rotary Encoder หมุนเลือกโหมด LED (ช่วง 0 ถึง 3)
  int ledMode = kit.updateMenuIndex(lastEncoderVal, 0, 3, false);
  lastEncoderVal = ledMode;

  kit.clearDisplay();
  kit.printText("=== ENCODER LED ===", 5, 5, 1);
  kit.printText(("Mode: " + String(ledMode)).c_str(), 15, 25, 2);

  // ให้นักเรียนเติมตรรกะเงื่อนไขตามโหมดที่เลือก
  if (ledMode == 0) {
    kit.printText("Status: ALL OFF", 15, 50, 1);
    // โจทย์ข้อ 1. ให้นักเรียนเขียนคำสั่ง ปิดไฟ LED ทั้งหมด
    
  } else if (ledMode == 1) {
    kit.printText("Status: RED LED", 15, 50, 1);
    // โจทย์ข้อ 2. ให้นักเรียนเขียนคำสั่ง เปิดไฟ LED สีแดง
    
  } else if (ledMode == 2) {
    kit.printText("Status: Yellow LED", 8, 50, 1);
    // โจทย์ข้อ 3. ให้นักเรียนเขียนคำสั่ง เปิดไฟ LED สีเหลือง
    
  } else {
    kit.printText("Status: GREEN LED", 10, 50, 1);
    // โจทย์ข้อ 4. ให้นักเรียนเขียนคำสั่ง : เปิดไฟ LED สีเขียว
  }
  
  kit.displayUpdate();
  delay(150);
}