/*
  MiniMicroKit_PTL - Red LED Preferences Save with Split Screen UI
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <Preferences.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;
Preferences pref;

int redPWM = 128; // ค่าความสว่างเริ่มต้น (0 - 255)
unsigned long uiTimer = 0;

void setup() {
  kit.begin();
  
  pinMode(PTL_LED_RED, OUTPUT);

  // โหลดค่าเดิมที่เคยบันทึกไว้ใน Flash Memory
  pref.begin("led_data", true);
  redPWM = pref.getInt("red_pwm", 128);
  pref.end();

  // อัปเดตค่าความสว่าง LED จริงทันทีที่เปิดเครื่อง
  analogWrite(PTL_LED_RED, redPWM);
}

void loop() {
  kit.update();
  EncoderButtonEvent btn = kit.getEncoderButton();

  // หมุน Encoder เพื่อปรับค่า PWM (0 - 255)
  int newPWM = kit.updateMenuIndex(redPWM, 0, 255, false);
  if (newPWM != redPWM) {
    redPWM = newPWM;
    analogWrite(PTL_LED_RED, redPWM);
    kit.beepAsync(30);
  }

  // กดปุ่ม Encoder เพื่อบันทึกค่าลงหน่วยความจำ Flash
  if (btn == ENC_BTN_CLICKED) {
    pref.begin("led_data", false);
    pref.putInt("red_pwm", redPWM);
    pref.end();

    kit.playSystemSound(SOUND_SUCCESS);
    kit.showDialog("SAVED!", "PWM: " + String(redPWM), 1000);
  }

  // ส่วนแสดงผลหน้าจอ OLED (แบ่ง 60% : 40%)
  if (!kit.isOLEDPowered()) return;

  if (kit.every(uiTimer, 50)) {
    kit.clearDisplay();

    // วาดเส้นแบ่งหน้าจอแนวตั้ง (ที่ตำแหน่ง X = 77)
    kit.display.drawLine(77, 0, 77, 63, SSD1306_WHITE);

    // ฝั่งซ้าย (60% พื้นที่ X = 0 ถึง 76)
    kit.display.setTextSize(1);
    kit.display.setCursor(3, 0);
    kit.display.print("SAVE LED PWM");

    // ตัวเลขตรงกลาง (ขนาด 2) จัดกึ่งกลางอัตโนมัติ
    String pwmStr = String(redPWM);
    kit.display.setTextSize(2);
    int numWidth = pwmStr.length() * 12; 
    int numX = (77 - numWidth) / 2;      
    kit.display.setCursor(numX, 18);
    kit.display.print(pwmStr);

    // ตัวอักษร "PWM Value" ใต้ตัวเลข (ขนาด 1)
    kit.display.setTextSize(1);
    kit.display.setCursor(11, 38);
    kit.display.print("PWM Value");

    // ด้านล่างสุด "Press Save" (ขนาด 1)
    kit.display.setCursor(7, 55);
    kit.display.print("Press Save");

    // ฝั่งขวา (40% พื้นที่ X = 78 ถึง 127)
    kit.display.setTextSize(2);
    
    // บรรทัดบน "RED"
    kit.display.setCursor(88, 18);
    kit.display.print("RED");

    // บรรทัดล่าง "LED"
    kit.display.setCursor(88, 38);
    kit.display.print("LED");

    kit.displayUpdate();
  }
}