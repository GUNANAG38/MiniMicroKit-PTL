/*
  Worksheet Pro
  LAB 9 : WS_Pro_LEDSelector
  โปรแกรม : ระบบเลือกเปิดไฟ LED ด้วย Encoder
  โจทย์เงื่อนไข : เขียนคำสั่งควบคุม LED และแสดงผลตามตำแหน่ง (ledIndex)
  คำแนะนำ : ให้นักเรียนเขียนเงื่อนไขคำสั่งควบคุม LED และแสดงผลตามตำแหน่ง (ledIndex)
*/

#include "MiniMicroKit_PTL.h"
MiniMicroKit_PTL kit;
int ledIndex = 0;      // 0=Red, 1=Yellow, 2=Green
int lastIndex = -1;    // ตัวแปรเก็บตำแหน่งเดิมเพื่อเช็คการเปลี่ยนแปลง

void setup() {
  kit.begin();
}

void loop() {
  kit.update();
  
  // ช่วงค่า : 0 ถึง 2, แบบวนลูป (true)
  ledIndex = kit.updateMenuIndex(ledIndex, 0, 2, true);

  // ตรวจสอบว่าเมนูเปลี่ยนหรือไม่ ก่อนสั่งวาดหน้าจอใหม่
  if (ledIndex != lastIndex) {
    kit.clearDisplay();
    kit.printText("=== LED SELECTOR ===", 2, 2, 1);
    
    // ให้นักเรียนเขียนเงื่อนไขคำสั่งควบคุม LED และแสดงผลตามตำแหน่ง (ledIndex)
    // โจทย์ข้อ 1. ถ้า ledIndex == 0 แสดง "Red" และใช้ kit.setLED แสดงผลสีแดง
    // โจทย์ข้อ 2. ถ้า ledIndex == 1 แสดง "Yellow" และใช้ kit.setLED แสดงผลสีเหลือง
    // โจทย์ข้อ 3. ถ้า ledIndex == 2 แสดง "Green" และใช้ kit.setLED แสดงผลสีเขียว
    
    kit.displayUpdate();
    lastIndex = ledIndex; // บันทึกตำแหน่งปัจจุบัน
  }
}