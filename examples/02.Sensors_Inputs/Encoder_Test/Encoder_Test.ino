#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

void setup() {
  Serial.begin(115200);
  delay(1000);
  kit.begin();
  
  // บังคับกำหนดพินปุ่มกดเป็น Pull-up เพื่อความชัวร์
  pinMode(PTL_ENC_SW, INPUT_PULLUP);
  
  Serial.println("=== SYSTEM DEBUGGER START ===");
}

void loop() {
  kit.update();

  // อ่านค่าสถานะดิบของขาต่างๆ
  int btnState = digitalRead(PTL_ENC_SW);
  int clkState = digitalRead(PTL_ENC_CLK);
  int dtState = digitalRead(PTL_ENC_DT);

  // พิมพ์ค่าออก Serial Monitor ทุกๆ 200 มิลลิวินาที
  static unsigned long debugTimer = 0;
  if (kit.every(debugTimer, 200)) {
    Serial.print("BTN (GPIO 0): ");
    Serial.print(btnState);
    Serial.print(" | CLK (GPIO 4): ");
    Serial.print(clkState);
    Serial.print(" | DT (GPIO 3): ");
    Serial.println(dtState);
  }
}