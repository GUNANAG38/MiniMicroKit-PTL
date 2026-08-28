/*
  MiniMicroKit_PTL - 3-Channel LED PWM Control
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// กำหนดสถานะหน้าจอ
enum AppState {
  STATE_MAIN_MENU,
  STATE_EDIT_RED,
  STATE_EDIT_YELLOW,
  STATE_EDIT_GREEN
};
AppState currentState = STATE_MAIN_MENU;

// ตัวแปรสำหรับเมนูหลัก (0 ถึง 2)
int menuIndex = 0;

// ค่าความสว่าง LED แต่ละช่อง (0 ถึง 51, คูณ 5 ได้ค่า PWM 0 ถึง 255)
int redStep = 25;    // ~125 (50%)
int yellowStep = 25; // ~125 (50%)
int greenStep = 25;  // ~125 (50%)

// Pin Assignments (ปรับเปลี่ยนขาตามฮาร์ดแวร์จริงของคุณ)
const int PIN_RED    = 5;
const int PIN_YELLOW = 6;
const int PIN_GREEN  = 7;

unsigned long uiTimer = 0;

// ฟังก์ชันอัปเดตค่า PWM ออกฮาร์ดแวร์จริงทั้ง 3 ช่อง
void updateHardwarePWM() {
  analogWrite(PIN_RED,    redStep * 5);
  analogWrite(PIN_YELLOW, yellowStep * 5);
  analogWrite(PIN_GREEN,  greenStep * 5);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  kit.begin();
  kit.setAutoSleep(true, 30000);

  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);

  updateHardwarePWM();

  if (kit.isOLEDPowered()) {
    kit.showDialog("PWM CONTROLLER", "3-CH LED Control", 1000);
  }
}

void loop() {
  kit.update();
  EncoderButtonEvent btn = kit.getEncoderButton();

  switch (currentState) {

    // หน้าเมนูหลัก (เลือกโหมด LED 1, 2, 3)
    case STATE_MAIN_MENU: {
      int newMenu = kit.updateMenuIndex(menuIndex, 0, 2, true);
      
      if (newMenu != menuIndex) {
        menuIndex = newMenu;
        kit.beepAsync(30); // เสียง Beep ตอบรับการหมุนเลื่อนเมนู
      }

      if (btn == ENC_BTN_CLICKED) {
        kit.playSystemSound(SOUND_CLICK); // เสียงคลิกปุ่ม SW
        
        if (menuIndex == 0) currentState = STATE_EDIT_RED;
        else if (menuIndex == 1) currentState = STATE_EDIT_YELLOW;
        else if (menuIndex == 2) currentState = STATE_EDIT_GREEN;
      }
      break;
    }

    // โหมดปรับ RED LED PWM (+5 / -5)
    case STATE_EDIT_RED: {
      int newStep = kit.updateMenuIndex(redStep, 0, 51, false);
      
      if (newStep != redStep) {
        redStep = newStep;
        updateHardwarePWM();
        kit.beepAsync(30); // เสียง Beep ตอนหมุนปรับเพิ่ม-ลดค่า PWM
      }

      if (btn == ENC_BTN_CLICKED || btn == ENC_BTN_LONG_PRESS) {
        kit.playSystemSound(SOUND_SUCCESS); // เสียงสำเร็จตอนออกจากโหมด
        currentState = STATE_MAIN_MENU;
      }
      break;
    }

    // โหมดปรับ YELLOW LED PWM (+5 / -5)
    case STATE_EDIT_YELLOW: {
      int newStep = kit.updateMenuIndex(yellowStep, 0, 51, false);
      
      if (newStep != yellowStep) {
        yellowStep = newStep;
        updateHardwarePWM();
        kit.beepAsync(30);
      }

      if (btn == ENC_BTN_CLICKED || btn == ENC_BTN_LONG_PRESS) {
        kit.playSystemSound(SOUND_SUCCESS);
        currentState = STATE_MAIN_MENU;
      }
      break;
    }

    // โหมดปรับ GREEN LED PWM (+5 / -5)
    case STATE_EDIT_GREEN: {
      int newStep = kit.updateMenuIndex(greenStep, 0, 51, false);
      
      if (newStep != greenStep) {
        greenStep = newStep;
        updateHardwarePWM();
        kit.beepAsync(30);
      }

      if (btn == ENC_BTN_CLICKED || btn == ENC_BTN_LONG_PRESS) {
        kit.playSystemSound(SOUND_SUCCESS);
        currentState = STATE_MAIN_MENU;
      }
      break;
    }
  }

  // ส่วนแสดงผลหน้าจอ OLED
  if (!kit.isOLEDPowered()) return;

  if (kit.every(uiTimer, 50)) {
    kit.clearDisplay();

    if (currentState == STATE_MAIN_MENU) {
      kit.printText("SELECT LED", 35, 0, 1);
      kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

      const char* menuNames[] = {
        "1. RED LED PWM",
        "2. YELLOW LED PWM",
        "3. GREEN LED PWM"
      };

      for (int i = 0; i < 3; i++) {
        int yPos = 16 + (i * 14);
        if (i == menuIndex) {
          kit.display.fillRect(0, yPos - 2, 128, 13, SSD1306_WHITE);
          kit.display.setTextColor(SSD1306_BLACK);
          kit.display.setCursor(6, yPos);
          kit.display.print(menuNames[i]);
          kit.display.setTextColor(SSD1306_WHITE);
        } else {
          kit.printText(menuNames[i], 6, yPos, 1);
        }
      }
    } 
    else {
      const char* title = "";
      int currentStepVal = 0;

      if (currentState == STATE_EDIT_RED) {
        title = "RED LED PWM";
        currentStepVal = redStep;
      } else if (currentState == STATE_EDIT_YELLOW) {
        title = "YELLOW LED PWM";
        currentStepVal = yellowStep;
      } else if (currentState == STATE_EDIT_GREEN) {
        title = "GREEN LED PWM";
        currentStepVal = greenStep;
      }

      int pwmVal = currentStepVal * 5;
      if (pwmVal > 255) pwmVal = 255;
      int percent = map(pwmVal, 0, 255, 0, 100);

      kit.printText(title, 0, 0, 1);
      kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

      kit.printText("PWM Value: " + String(pwmVal) + " / 255", 0, 16, 1);
      kit.printText("Resolution: Step +/-5", 0, 28, 1);

      // Bar กราฟแสดงระดับ PWM
      kit.drawProgressBar(0, 40, 128, 12, percent);

      kit.printText("[Click to Back]", 20, 56, 1);
    }

    kit.displayUpdate();
  }
}