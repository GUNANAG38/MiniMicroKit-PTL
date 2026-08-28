/*
  Worksheet simple
  LAB 2 : WS_simple_OLEDShape
  โปรแกรม : การวาดเส้น กรอบ และรูปทรงเรขาคณิตบนจอ OLED
  โจทย์เงื่อนไข : ใช้งานคำสั่งกราฟิกของจอ OLED เพื่อวาดกรอบสี่เหลี่ยม เส้นตรง และวงกลม
  คำแนะนำ : ให้นักเรียนเติมคำสั่งวาดรูปทรงเรขาคณิตเพิ่มเติมตามโจทย์
*/

#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

void setup() {
  kit.begin();
}

void loop() {
  kit.update();
  kit.clearDisplay(); // ล้างหน้าจอก่อนวาดภาพใหม่
  
  // วาดกรอบสี่เหลี่ยมรอบหน้าจอ drawRect(x, y, width, height, color)
  kit.display.drawRect(0, 0, 128, 64, SSD1306_WHITE);

  // ให้นักเรียนเติมคำสั่งกราฟิกเพิ่มเติมตามโจทย์
  // โจทย์ข้อ 1. ให้นักเรียนเขียนคำสั่งวาดเส้นตรงแนวนอน kit.display.drawLine(x0, y0, x1, y1, color);
  // โจทย์ข้อ 2. ให้นักเรียนเขียนคำสั่งวาดวงกลมหรือสี่เหลี่ยมทึบ kit.display.drawCircle(...) หรือ fillRect(...)
  
  kit.displayUpdate(); // อัปเดตข้อมูลแสดงผลออกทางจอ OLED
  delay(1000);
}