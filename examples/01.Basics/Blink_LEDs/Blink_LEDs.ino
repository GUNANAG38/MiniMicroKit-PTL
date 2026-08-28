/*
MiniMicroKit_PTL - Comprehensive LED Control Example
  
  Demonstrates all LED control functions available in the library:
  - Direct individual LED control (Red, Yellow, Green)
  - Turning all LEDs OFF
  - Background Async Non-blocking Blinking
  - Non-blocking LED Cycling using kit.every()
  
  Developer: GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// ตัวแปร Timers สำหรับทดสอบการสลับไฟใน loop()
unsigned long ledTimer = 0;
int ledStateStep = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  kit.begin();
  Serial.println("[INFO] MiniMicroKit_PTL LED Demo Started");

  // รูปแบบของตำแหน่งหลอดไฟ LED (แดง/เหลือง/เขียว)


  // สั่งเปิดไฟสีแดงดวงเดียว (เหลือง/เขียว ดับ)
  kit.setLED(true, false, false); 
  delay(1000);

  // สั่งดับไฟ LED ทุกดวงพร้อมกัน
  kit.setLED(false, false, false);
  delay(1000);

  // สั่งกระพริบไฟ แบบ Non-blocking (Active Default)
  // พารามิเตอร์ : blinkLED(Red, Yellow, Green, Interval_ms)
  kit.blinkLED(true, true, true, 300); // กระพริบทั้ง 3 ดวง ทุกๆ 300ms
}

void loop() {
  // ต้องเรียก kit.update() ใน loop() เสมอ เพื่อให้ไฟกระพริบ (Async Blink)
  kit.update();

  // การสลับไฟวิ่งเรียงลำดับแบบ Non-blocking ใน loop()
  // (หากต้องการทดสอบ ให้ปิดบรรทัด kit.blinkLED() ใน setup() ก่อน แล้วเปิดคอมเมนต์ในชุดนี้)
  
  /*
  if (kit.every(ledTimer, 1000)) { // สลับไฟทุกๆ 1 วินาที (1000ms)
    switch (ledStateStep) {
      case 0:
        kit.setLED(true, false, false);  // ติดสีแดง
        break;
      case 1:
        kit.setLED(false, true, false);  // ติดสีเหลือง
        break;
      case 2:
        kit.setLED(false, false, true);  // ติดสีเขียว
        break;
      case 3:
        kit.setLED(false, false, false); // ดับทุกดวง
        break;
    }
    ledStateStep = (ledStateStep + 1) % 4;
  }
  */
}