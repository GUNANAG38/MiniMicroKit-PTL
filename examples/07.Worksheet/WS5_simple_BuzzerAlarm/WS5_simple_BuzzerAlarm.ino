/*
  Worksheet simple
  LAB 5 : WS_simple_BuzzerAlarm
  โปรแกรม : ระบบสัญญาณเตือนภัยอัจฉริยะด้วย Buzzer แสดงผลผ่านเสียง, หลอดไฟ LED และจอ OLED 
  โจทย์เงื่อนไข :  ใช้ปุ่มกดที่ตัว Rotary Encoder เพื่อสลับเปิด-ปิดระบบสัญญาณเสียงเตือน (Buzzer) 
                แสดงสถานะบนจอ OLED ,หลอดไฟ LED สีแดง และควบคุมเสียงเตือนตามสถานะที่เลือก
  คำแนะนำ : ให้นักเรียนเติมเต็มตรรกะในบล็อกเงื่อนไข if เพื่อตรวจสอบสถานะ alarmState และสั่งเล่นเสียงเตือนหรือปิดเสียงตามต้องการ
*/

#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;
bool alarmState = false; // ตัวแปรเก็บสถานะเปิด/ปิดสัญญาณเตือน

void setup() {
  kit.begin();
}

void loop() {
  kit.update();
  
  // ตรวจจับการกดปุ่ม Encoder เพื่อสลับสถานะ Alarm
  if (kit.getEncoderButton() == ENC_BTN_CLICKED) {
    alarmState = !alarmState; // สลับสถานะ (true <-> false)
    kit.playSystemSound(SOUND_CLICK);
  }

  kit.clearDisplay();
  kit.printText("=== ALARM SYSTEM ===", 2, 5, 1);

  // ให้นักเรียนเติมตรรกะเงื่อนไข หาก alarmState เป็นจริง
  if (alarmState == true) {
    kit.printText("Status: ALARM ON!", 10, 35, 1);
    // โจทย์ข้อ 1. ให้นักเรียนเขียนคำสั่ง สั่งเล่นเสียงเตือน และเปิดไฟ LED สีแดง
    
  } else {
    kit.printText("Status: ALARM OFF", 10, 35, 1);
    // โจทย์ข้อ 2. ให้นักเรียนเขียนคำสั่ง ปิดเสียงเตือนและสถานะปกติ
  }
  
  kit.displayUpdate();
  delay(100);
}