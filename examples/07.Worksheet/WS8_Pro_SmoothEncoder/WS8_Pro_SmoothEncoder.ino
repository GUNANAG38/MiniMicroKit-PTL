/*
  Worksheet Pro
  LAB 8 : WS_Pro_SmoothEncoder
  โปรแกรม : ระบบปรับค่าความไวแสงและตั้งค่าด้วย Encoder แบบไม่หน่วง
  โจทย์เงื่อนไข : เขียนคำสั่งแสดงผลค่า Smooth บนจอ OLED พิกัด X=10, Y=30, ขนาดตัวอักษร = 2
  คำแนะนำ : ให้นักเรียนเขียนคำสั่งแสดงผลค่า Smooth บนจอ OLED
*/

#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;
int smooth = 50; 
int lastSmooth = -1; // ตัวแปรเก็บค่าเดิมไว้เช็คการเปลี่ยนแปลง

void setup() {
  kit.begin();
}

void loop() {
  kit.update();
  
  // อ่านค่าจาก Encoder ทันทีทุกรอบลูปเพื่อให้ตอบสนองไว
  smooth = kit.updateMenuIndex(smooth, 0, 100, false);

  // ตรวจสอบว่าค่าเปลี่ยนไปจากเดิมหรือไม่ ก่อนสั่งวาดหน้าจอใหม่
  if (smooth != lastSmooth) {
    kit.clearDisplay();
    
    kit.printText("== SMOOTH SETTING ==", 2, 2, 1);
    
    // ให้นักเรียนเขียนคำสั่งแสดงผลค่า Smooth บนจอ OLED
    // โจทย์ข้อ 1 : ให้นักเรียนเขียนคำสั่งแสดงผลข้อความพิกัด X=10, Y=30, ขนาดตัวอักษร = 2
     
    kit.displayUpdate();
    
    // บันทึกค่าปัจจุบันเก็บไว้เทียบในรอบถัดไป
    lastSmooth = smooth; 
  }
}