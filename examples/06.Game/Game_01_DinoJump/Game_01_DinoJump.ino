#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

int playerY = 44;
int jumpVel = 0;
bool isJumping = false;

int obsX = 128;
int obsW = 6;
int obsH = 10;

unsigned long score = 0;
bool gameOver = false;

void resetGame() {
  playerY = 44;
  jumpVel = 0;
  isJumping = false;
  obsX = 128;
  score = 0;
  gameOver = false;
  kit.setLED(false, false, true); // เปิดไฟเขียวสถานะปกติ
}

void setup() {
  Serial.begin(115200);
  kit.begin();
  resetGame();
}

void loop() {
  kit.update(); // ประมวลผลระบบภายในไลบรารี

  if (gameOver) {
    kit.clearDisplay();
    kit.printText("=== GAME OVER ===", 12, 10, 1);
    kit.printText(("Score : " + String(score)).c_str(), 33, 25, 1);
    kit.printText("Press to Retry", 23, 45, 1);
    kit.displayUpdate();

    // ใช้ฟังก์ชันตรวจสอบการกดปุ่มจากไลบรารี (PTL_ENC_SW / GPIO 0)
    if (kit.getEncoderButton() == ENC_BTN_CLICKED) {
      kit.playSystemSound(SOUND_CLICK);
      resetGame();
    }
    return;
  }

  // ควบคุมการกระโดดด้วยปุ่มกด Encoder Switch
  if (kit.getEncoderButton() == ENC_BTN_CLICKED && !isJumping) {
    isJumping = true;
    jumpVel = -6; // แรงส่งขึ้นด้านบน
    kit.playSystemSound(SOUND_LASER);
  }

  // ระบบการกระโดด
  if (isJumping) {
    playerY += jumpVel;
    jumpVel += 1; // แรงโน้มถ่วง
    if (playerY >= 44) {
      playerY = 44;
      isJumping = false;
    }
  }

  // การเคลื่อนที่ของสิ่งกีดขวาง
  obsX -= 4;
  if (obsX < -obsW) {
    obsX = 128;
    score += 10;
    kit.playSystemSound(SOUND_SUCCESS); // เสียงเมื่อผ่านด่าน/ได้แต้ม
  }

  // ตรวจสอบการชน (Collision Detection)
  if (obsX <= 21 && obsX + obsW >= 15 && playerY + 8 >= 54 - obsH) {
    gameOver = true;
    kit.playSystemSound(SOUND_ERROR); // เสียงเมื่อชน
    kit.setLED(true, false, false);   // เปิดไฟแดงเตือนสถานะ Game Over
  }

  // แสดงผลบนจอ OLED
  kit.clearDisplay();
  kit.display.drawLine(0, 54, 127, 54, 1); // เส้นพื้น
  kit.display.fillRect(15, playerY, 6, 8, 1);  // ตัวละครผู้เล่น
  kit.display.fillRect(obsX, 54 - obsH, obsW, obsH, 1); // สิ่งกีดขวาง
  kit.printText(("Score:" + String(score)).c_str(), 0, 0, 1);
  kit.displayUpdate();
  
  delay(40);
}