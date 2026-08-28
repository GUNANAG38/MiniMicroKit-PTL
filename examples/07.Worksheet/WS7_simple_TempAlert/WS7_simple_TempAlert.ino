/*
  Worksheet simple
  LAB 7 : WS_simple_TempAlert
  โปรแกรม : อ่านค่าอุณหภูมิโดยใช้ DHT11 Mini (ตามอุณหภูมิห้อง) แสดงผลค่าอุณหภูมิบนจอ OLED 
  โจทย์เงื่อนไข : หากค่าอุณหภูมิ เกินค่าที่ตั้งไว้ เช่น 30 องศาเซลเซียส ให้สั่งเปิดไฟ LED สีแดงพร้อมส่งเสียงเตือน แต่ถ้าปกติให้แสดงไฟสีเขียว
  คำแนะนำ : ให้นักเรียนเติมเต็มตรรกะในบล็อกเงื่อนไข if เพื่อควบคุมสถานะของ LED และเสียงเตือนตามที่โจทย์กำหนด
*/

#include <DHT.h>
#include "MiniMicroKit_PTL.h"

#define DHTPIN 21     // เปลี่ยนมาใช้ขา 21 ตามที่ต่อจริง
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
MiniMicroKit_PTL kit;

void setup() {
  kit.begin();
  dht.begin();
}
void loop() {
  kit.update();
  float temp = dht.readTemperature(); // อ่านค่าอุณหภูมิจริงจาก DHT11 Mini

  kit.clearDisplay();
  kit.printText("=== TEMP ALERT ===", 10, 5, 1);
  kit.printText(("Temp:" + String(temp,0) + " C").c_str(), 10, 25, 2);

  // ให้นักเรียนเติมตรรกะเงื่อนไข หาก temp เกิน 30 องศา
  if (temp > 30) {
    kit.printText("Status: OVERHEAT!", 16, 50, 1);
    // โจทย์ข้อ 1. ให้นักเรียนเขียนคำสั่ง เปิดไฟ LED สีแดง และเล่นเสียงเตือน (SOUND_ERROR)
    
  } else {
    kit.printText("Status: Normal", 23, 50, 1);
    // โจทย์ข้อ 2. ให้นักเรียนเขียนคำสั่ง สั่งเปิดไฟ LED สีเขียวปกติ
  }
  kit.displayUpdate();
  delay(150);
}