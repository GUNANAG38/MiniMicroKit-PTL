/*
  Sound Test Tool - ทดสอบเสียง Active Buzzer ผ่าน Encoder
*/
#include "MiniMicroKit_PTL.h"
MiniMicroKit_PTL kit;
int soundIndex = 0;
int lastIndex = -1;

void setup() {
  kit.begin();
}

void loop() {
  kit.update();
  
  // หมุนเลือกเสียง 0 ถึง 18 (รวม 19 เสียง) แบบวนลูป
  soundIndex = kit.updateMenuIndex(soundIndex, 0, 18, true);

  // กดปุ่มเพื่อเล่นเสียงที่เลือก
  if (kit.getEncoderButton() == ENC_BTN_CLICKED) {
    kit.playSystemSound((SoundType)soundIndex);
  }

  // อัปเดตหน้าจอเมื่อเปลี่ยนรายการ
  if (soundIndex != lastIndex) {
    kit.clearDisplay();
    kit.printText("=== SOUND TEST ===", 10, 2, 1);
    
    // รายชื่อเสียงทั้งหมด 
    String sNames[] = {
      "1. CLICK", "2. SUCCESS", "3. ERROR", "4. STARTUP",
      "5. CONFIRM", "6. CANCEL", "7. WARNING", "8. ALARM",
      "9. NOTIFY", "10. LEVEL UP", "11. PWR OFF", "12. LONG BEEP",
      "13. DOUBLE", "14. HAPPY" , "15. CONNECT", "16. DISCONNECT",
      "17. LASER", "18. COIN" , "19. HEARTBEAT" 
    };
    
    kit.printText("> " + sNames[soundIndex], 10, 30, 1);
    kit.printText("Press to Play", 15, 50, 1);
    kit.displayUpdate();
    lastIndex = soundIndex;
  }
}