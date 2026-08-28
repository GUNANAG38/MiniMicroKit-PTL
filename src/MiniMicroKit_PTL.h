#ifndef MINI_MICRO_KIT_PTL_H
#define MINI_MICRO_KIT_PTL_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PIN (PT-L ESP32-C3 Supermini Board)
#define PTL_OLED_SDA      2   // GPIO2
#define PTL_OLED_SCL      1   // GPIO1

#define PTL_ENC_CLK       4   // GPIO4
#define PTL_ENC_DT        3   // GPIO3
#define PTL_ENC_SW        0   // GPIO0 (DIP Switch = SW)

#define PTL_LED_RED       5   // GPIO5 (DIP Switch = LED Red)
#define PTL_LED_YELLOW    6   // GPIO6 (DIP Switch = LED Yellow)
#define PTL_LED_GREEN     7   // GPIO7 (DIP Switch = LED Green)

#define PTL_BUZZER        10  // GPIO10

enum EncoderButtonEvent {
  ENC_BTN_NONE,
  ENC_BTN_CLICKED,
  ENC_BTN_LONG_PRESS
};

enum SoundType {
  SOUND_CLICK,
  SOUND_SUCCESS,
  SOUND_ERROR,
  SOUND_STARTUP,
  SOUND_CONFIRM,
  SOUND_CANCEL,
  SOUND_WARNING,
  SOUND_ALARM,
  SOUND_NOTIFY,
  SOUND_LEVEL_UP,
  SOUND_POWER_OFF,
  SOUND_BEEP_LONG,
  SOUND_DOUBLE_CLICK, 
  SOUND_HAPPY,
  SOUND_CONNECT,
  SOUND_DISCONNECT,
  SOUND_LASER,
  SOUND_COIN,
  SOUND_HEARTBEAT
};

class MiniMicroKit_PTL {
public:
  Adafruit_SSD1306 display;

  MiniMicroKit_PTL();
  bool begin(int sda = PTL_OLED_SDA, int scl = PTL_OLED_SCL);
  void update();

  // LED Controls (Active-LOW)
  void setLED(bool red, bool yellow, bool green);
  void blinkLED(bool red, bool yellow, bool green, unsigned long intervalMs);

  // Sound Controls
  void beep(unsigned int frequency, unsigned long durationMs);
  void beepAsync(unsigned long durationMs);
  void playSystemSound(SoundType type);

  // Rotary Encoder and Button
  int updateMenuIndex(int current, int minVal, int maxVal, bool wrap = true);
  EncoderButtonEvent getEncoderButton();

  // OLED Helpers
  void clearDisplay();
  void displayUpdate();
  void printText(String text, int x, int y, int size = 1);
  void drawProgressBar(int x, int y, int width, int height, int percentage);
  void showDialog(String title, String message, unsigned long displayMs = 1000);
  void setAutoSleep(bool enable, unsigned long timeoutMs = 15000);
  bool isOLEDPowered();

  // Utilities
  void scanI2CBus(bool printToSerial = true);
  int readAnalogSmooth(uint8_t pin, float alpha = 0.2f);
  bool every(unsigned long &timer, unsigned long intervalMs);

private:
  int _sdaPin, _sclPin;
  int _lastCLKState;
  
  // Debounce and Long Press Variables for GPIO 0
  unsigned long _lastBtnPressTime;
  bool _lastBtnState;
  bool _isLongPressHandled;

  // Auto Sleep and Async States
  unsigned long _lastActivityTime;
  unsigned long _autoSleepTimeout;
  bool _autoSleepEnabled;
  bool _oledPowered;

  bool _ledBlinkActive;
  unsigned long _ledBlinkTimer;
  unsigned long _ledBlinkInterval;
  bool _ledBlinkState;

  unsigned long _asyncBeepTimer;
  bool _asyncBeepActive;

  bool initOLEDAuto();
  void wakeupOLED();
};

#endif