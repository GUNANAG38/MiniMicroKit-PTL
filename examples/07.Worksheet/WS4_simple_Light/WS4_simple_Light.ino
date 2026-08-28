/*
  Worksheet simple
  LAB 4 : WS_simple_Light
  โปรแกรม : อ่านค่าสัญญาณอนาล็อกความเข้มแสงจากเซ็นเซอร์ LDR แสดงผลค่าความเข้มแสงบนจอ OLED 
  โจทย์เงื่อนไข : หากค่าความเข้มแสง น้อยกว่า 300 (สภาพแวดล้อมเริ่มมืด) 
               ให้สั่งเปิดไฟ LED ส่องสว่างพร้อมแสดงข้อความ "DARK" 
               แต่ถ้าแสงปกติให้แสดงข้อความ "BRIGHT" และปิดไฟ
  คำแนะนำ : ให้นักเรียนเติมเต็มตรรกะในบล็อกเงื่อนไข if เพื่อควบคุมการทำงานของไฟ LED ตามสภาพแสง
*/

#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

void setup() {
  kit.begin();
}

void loop() {
  kit.update();
  int lightVal = analogRead(A0); // อ่านค่าความเข้มแสงจาก LDR
  
  kit.clearDisplay();
  kit.printText("=== LIGHT SENSOR ===", 5, 5, 1);
  kit.printText(("LUX:" + String(lightVal)).c_str(), 14, 25, 2);

  // ให้นักเรียนเติมตรรกะเงื่อนไข หากค่าแสงน้อยกว่า 300 ถือว่ามืด
  if (lightVal < 300) {
    kit.printText("Status: DARK", 28, 50, 1);
    // โจทย์ข้อ 1. ให้นักเรียนเขียนคำสั่ง สั่งเปิดไฟ LED ส่องสว่าง (LED หลอดสีเหลือง)

  } else {
    kit.printText("Status: BRIGHT", 22, 50, 1);
    // โจทย์ข้อ 2. ให้นักเรียนเขียนคำสั่ง สั่งปิดไฟ (LED หลอดสีเหลือง)
  }

  kit.displayUpdate();
  delay(100);
}