#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

int paddleY = 24;
int ballX = 64;
int ballY = 32;
int ballDirX = -2;
int ballDirY = 1;

unsigned long score = 0;
bool gameOver = false;
unsigned long gameTimer = 0; 

// ตัวแปรสำหรับจับการเปลี่ยนแปลงและเร่งความเร็ว Encoder
int lastEncoderVal = 24;

void resetGame() {
  paddleY = 24;
  lastEncoderVal = 24;
  ballX = 64;
  ballY = 32;
  ballDirX = -2;
  ballDirY = 1;
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
  // เรียก update ของไลบรารีตลอดเวลาเพื่อให้จับสัญญาณพัลส์ได้แม่นยำ
  kit.update();

  // ระบบเร่งความเร็วการตอบสนองของ Encoder (Sensitivity Multiplier)
  int currentEncoderVal = kit.updateMenuIndex(lastEncoderVal, 0, 48, false);
  int delta = currentEncoderVal - lastEncoderVal;
  if (delta != 0) {
    paddleY += delta * 3; // คูณ 3 เพื่อให้แป้นขยับไวขึ้นตามจังหวะการหมุน
    if (paddleY < 0) paddleY = 0;
    if (paddleY > 48) paddleY = 48; // จำกัดขอบเขตหน้าจอ OLED (64 - ความสูงแป้น 16)
    lastEncoderVal = currentEncoderVal;
  }

  if (gameOver) {
    kit.clearDisplay();
    kit.printText("=== GAME OVER ===", 12, 10, 1);
    kit.printText(("Score: " + String(score)).c_str(), 43, 25, 1);
    kit.printText("Press to Retry", 23, 45, 1);
    kit.displayUpdate();

    // กดปุ่มที่ Encoder เพื่อเริ่มเล่นใหม่
    if (kit.getEncoderButton() == ENC_BTN_CLICKED) {
      kit.playSystemSound(SOUND_CLICK);
      resetGame();
    }
    return;
  }

  // ควบคุมความเร็วเฟรมเรตเกมด้วย kit.every (ไม่บล็อกการทำงานของลูป)
  if (kit.every(gameTimer, 35)) {
    // การเคลื่อนที่ของลูกบอล
    ballX += ballDirX;
    ballY += ballDirY;

    // ชนขอบบนหรือขอบล่างของจอ
    if (ballY <= 0 || ballY >= 61) {
      ballDirY = -ballDirY;
      kit.playSystemSound(SOUND_CLICK);
    }

    // ชนขอบฝั่งขวาเด้งกลับ
    if (ballX >= 125) {
      ballDirX = -ballDirX;
      kit.playSystemSound(SOUND_CLICK);
    }

    // ตรวจสอบการชนแป้นพายฝั่งซ้าย
    if (ballX <= 8 && ballX >= 4) {
      if (ballY >= paddleY && ballY <= paddleY + 16) {
        ballDirX = -ballDirX;
        score += 1;
        kit.playSystemSound(SOUND_SUCCESS);
      }
    }

    // ลูกบอลหลุดออกนอกจอฝั่งซ้าย = Game Over
    if (ballX < 0) {
      gameOver = true;
      kit.playSystemSound(SOUND_ERROR);
      kit.setLED(true, false, false); // เปิดไฟแดงเตือน
    }

    // แสดงผลกราฟิกบนจอ OLED
    kit.clearDisplay();
    kit.display.fillRect(4, paddleY, 3, 16, 1);    // วาดแป้นพาย
    kit.display.fillRect(ballX, ballY, 3, 3, 1);      // วาดลูกบอล
    kit.printText(("Score:" + String(score)).c_str(), 40, 0, 1); // แสดงคะแนน
    kit.displayUpdate();
  }
}