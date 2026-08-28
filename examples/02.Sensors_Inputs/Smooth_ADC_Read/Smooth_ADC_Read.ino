/*
  MiniMicroKit_PTL - Interactive Smooth ADC (Knob Controls Alpha)
  
  Interactive Controls:
  - Rotate Knob    : Adjust Alpha Factor (0.01 - 1.00) dynamically
  - Short Click    : Step SIM Voltage (0.5V -> 1.5V -> 2.4V -> 3.3V) to test Step Response
  - Long Press     : Toggle Mode [SIM] <-> [REAL:GP3]
  
  Developer: GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

const int ADC_PIN = 3; // ขาอนาล็อก

// ตัวแปรโหมดการทำงาน
bool isSimMode = true; 

// การควบคุม Alpha ด้วยการหมุน (1 - 100 -> 0.01 - 1.00)
int alphaStep = 15; // ค่าเริ่มต้น Alpha = 0.15
float alphaVal = 0.15;

// ระดับแรงดันจำลองสำหรับทดสอบ Step Response (หน่วย ADC 0-4095)
int simPresetAdc[] = {620, 1860, 2970, 4095}; // ~0.5V, 1.5V, 2.4V, 3.3V
int simPresetIndex = 1; // เริ่มต้นที่ 1.5V

// ตัวแปรประมวลผล ADC
float rawAdc = 1860.0;
float filteredAdc = 1860.0;

// บัฟเฟอร์วาดกราฟ 128 พิกเซล
const int GRAPH_WIDTH = 128;
int graphHistory[GRAPH_WIDTH];

// Timers
unsigned long adcTimer = 0;
unsigned long uiTimer = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  kit.begin();
  analogReadResolution(12);

  // เติมค่าเริ่มต้นกราฟให้อยู่ตรงกลาง
  for (int i = 0; i < GRAPH_WIDTH; i++) graphHistory[i] = 44;

  kit.setAutoSleep(true, 20000);

  if (kit.isOLEDPowered()) {
    kit.showDialog("ADC FILTER", "Alpha Knob Mode", 1200);
  }
}

void loop() {
  kit.update();

  EncoderButtonEvent btn = kit.getEncoderButton();


  // หมุนปรับค่า Alpha + กดปุ่มเปลี่ยนแรงดัน/สลับโหมด
  // หมุนลูกบิด ปรับค่า Alpha (0.01 - 1.00) ได้อย่างต่อเนื่อง
  int newAlphaStep = kit.updateMenuIndex(alphaStep, 1, 100, false);
  if (newAlphaStep != alphaStep) {
    alphaStep = newAlphaStep;
    alphaVal = (float)alphaStep / 100.0; // แปลงสเต็ป 1..100 ให้เป็นค่า float 0.01..1.00
    kit.beepAsync(10);
  }

  // กดปุ่มสั้น สลับระดับแรงดันจำลองแบบกระโดด
  if (btn == ENC_BTN_CLICKED) {
    if (isSimMode) {
      simPresetIndex = (simPresetIndex + 1) % 4;
      kit.playSystemSound(SOUND_CLICK);
    }
  }
  
  // สลับโหมด REAL <-> SIM
  else if (btn == ENC_BTN_LONG_PRESS) {
    isSimMode = !isSimMode;
    
    // เคลียร์ค่าค้างเมื่อสลับโหมด
    if (isSimMode) {
      rawAdc = simPresetAdc[simPresetIndex];
      filteredAdc = rawAdc;
    } else {
      rawAdc = (float)analogRead(ADC_PIN);
      filteredAdc = rawAdc;
    }
    
    // บังคับ Sync ตำแหน่ง Encoder ภายในไลบรารี ให้ตรงกับค่า alphaStep เดิม
    kit.updateMenuIndex(alphaStep, 1, 100, false);
    kit.playSystemSound(SOUND_SUCCESS);
  }

  // การอ่านค่าสัญญาณ ADC / สุ่ม Noise / คำนวณ EMA
  if (kit.every(adcTimer, 10)) {
    if (isSimMode) {
      // โหมด : ใช้ค่าแรงดัน Preset + สุ่ม Noise รบกวน (+/- 180)
      rawAdc = (float)simPresetAdc[simPresetIndex] + random(-180, 180);
      rawAdc = constrain(rawAdc, 0, 4095);
    } else {
      // โหมด : อ่านจากขา GPIO3 จริง
      rawAdc = (float)analogRead(ADC_PIN);
    }

    // คำนวณ EMA Filter ตามค่า alphaVal ที่ได้จากการหมุนลูกบิด
    filteredAdc = (alphaVal * rawAdc) + ((1.0 - alphaVal) * filteredAdc);
  }

  // แสดงผลบนหน้าจอ OLED
  if (!kit.isOLEDPowered()) return;

  if (kit.every(uiTimer, 40)) {
    kit.clearDisplay();

    float volt = (filteredAdc / 4095.0) * 3.3;

    // แสดงโหมด และค่า Alpha ที่หมุนปรับได้
    String modeStr = isSimMode ? "[SIM]" : "[REAL]";
    kit.printText(modeStr, 0, 0, 1);
    kit.printText("Alpha:" + String(alphaVal, 2), 60, 0, 1);

    // ค่าแรงดัน V และ RAW ADC
    kit.printText("VOLT:" + String(volt, 2) + "V  RAW:" + String((int)rawAdc), 0, 11, 1);

    // เส้นแบ่งโซน
    kit.display.drawLine(0, 21, 127, 21, SSD1306_WHITE);

    // วาด Real-time Waveform Graph
    for (int i = 0; i < GRAPH_WIDTH - 1; i++) {
      graphHistory[i] = graphHistory[i + 1];
    }

    int yPixel = map((int)filteredAdc, 0, 4095, 63, 24);
    graphHistory[GRAPH_WIDTH - 1] = yPixel;

    for (int x = 0; x < GRAPH_WIDTH - 1; x++) {
      kit.display.drawLine(x, graphHistory[x], x + 1, graphHistory[x + 1], SSD1306_WHITE);
    }

    kit.displayUpdate();
  }
}