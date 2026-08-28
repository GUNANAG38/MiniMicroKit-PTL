/*
  Worksheet Pro
  LAB 10 : WS_Pro_OLED_ICONBitmap
  โปรแกรม : แสดงผลรูปภาพไอคอนกราฟิก รูปหัวใจ (Bitmap) บนจอ OLED
  โจทย์เงื่อนไข : คำนวณหาค่าด้วยเลขฐานและแก้ไข 0x00 เพื่อหาค่าบิตที่ใช้งาน
  คำแนะนำ : ให้นักเรียนเขียนคำสั่งแสดงผลภาพ Bitmap ให้สมบูรณ์
*/

#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// ข้อมูลอาเรย์รูปภาพไอคอนขนาด 16x16 พิกเซล (เก็บไว้ใน Flash Memory)
// รูปแบบคำสั่ง const unsigned char PROGMEM icon[] = {แก้ไขชุดเลขฐานสิบหกในแต่ละไบต์ภายในปีกกานี้}
// ให้นักเรียนเขียนคำสั่งและคำนวณหาค่าบิตที่ใช้งาน เพื่อแสดงผลภาพ Bitmap ตามโจทย์ที่กำหนด
// โจทย์ข้อ 1 : ให้นักเรียนคำนวณและแก้ไขชุดเลขฐานสิบหก เพื่อแสดงไอคอนรูปหัวใจ (Bitmap)
const unsigned char PROGMEM icon[] = 
{
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

void setup() {
  kit.begin();
}

void loop() {
  kit.update();
  kit.clearDisplay(); // ล้างหน้าจอก่อนวาดภาพใหม่
  
  kit.printText("=== BITMAP ICON ===", 10, 2, 1);

  // วาดรูปภาพ Bitmap ตามพิกัดและขนาดที่กำหนด
  // kit.display.drawBitmap(x, y, ชื่อตัวแปรอาเรย์ข้อมูลไอคอน, พิกเซล, พิกเซล, สี);
  kit.display.drawBitmap(56, 25, icon, 16, 16, SSD1306_WHITE);
  
  kit.displayUpdate(); // อัปเดตข้อมูลแสดงผลออกทางจอ OLED
  delay(1000);
}