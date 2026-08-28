#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL::MiniMicroKit_PTL() 
  : display(128, 64, &Wire, -1),
    _lastBtnPressTime(0),
    _lastBtnState(HIGH),
    _isLongPressHandled(false),
    _lastActivityTime(0),
    _autoSleepTimeout(15000),
    _autoSleepEnabled(false),
    _oledPowered(true),
    _ledBlinkActive(false),
    _ledBlinkState(false),
    _asyncBeepActive(false) {}

bool MiniMicroKit_PTL::begin(int sda, int scl) {
  _sdaPin = sda;
  _sclPin = scl;

  // Setup LED (Active-HIGH: HIGH = ติด, LOW = ดับ)
  pinMode(PTL_LED_RED, OUTPUT);
  pinMode(PTL_LED_YELLOW, OUTPUT);
  pinMode(PTL_LED_GREEN, OUTPUT);
  digitalWrite(PTL_LED_RED, LOW);   // ดับไฟทั้งหมดเมื่อเริ่มต้น
  digitalWrite(PTL_LED_YELLOW, LOW);
  digitalWrite(PTL_LED_GREEN, LOW);

  // Setup Buzzer
  pinMode(PTL_BUZZER, OUTPUT);
  digitalWrite(PTL_BUZZER, LOW);

  // Setup Rotary Encoder
  pinMode(PTL_ENC_CLK, INPUT_PULLUP);
  pinMode(PTL_ENC_DT, INPUT_PULLUP);
  pinMode(PTL_ENC_SW, INPUT_PULLUP);

  _lastCLKState = digitalRead(PTL_ENC_CLK);
  _lastActivityTime = millis();

  // เริ่มต้น I2C (SDA=GPIO2, SCL=GPIO1)
  delay(200);
  Wire.begin(_sdaPin, _sclPin);
  Wire.setClock(100000);

  bool oledOK = initOLEDAuto();
  if (oledOK) {
    playSystemSound(SOUND_STARTUP);
  }
  return oledOK;
}

// อ่านการหมุน Rotary Encoder พร้อม Debounce กรองสัญญาณรบกวน 8ms
int MiniMicroKit_PTL::updateMenuIndex(
    int current,
    int minVal,
    int maxVal,
    bool wrap
) {
    static unsigned long lastTurnTimer = 0;
    int clkState = digitalRead(PTL_ENC_CLK);
    if (clkState != _lastCLKState) {
        // ตรวจจับเฉพาะขอบขาลงของ CLK
        if (clkState == LOW && (millis() - lastTurnTimer > 8)) {
            // กลับทิศทาง Encoder
            if (digitalRead(PTL_ENC_DT) != clkState) {
                current--;
            } else {
                current++;
            }
            if (wrap) {
                if (current > maxVal) current = minVal;
                if (current < minVal) current = maxVal;
            } else {
                current = constrain(current, minVal, maxVal);
            }
            _lastActivityTime = millis();
            wakeupOLED();
            lastTurnTimer = millis();
        }
        _lastCLKState = clkState;
    }
    return current;
}

EncoderButtonEvent MiniMicroKit_PTL::getEncoderButton() {
  EncoderButtonEvent event = ENC_BTN_NONE;
  bool currentState = digitalRead(PTL_ENC_SW);

  if (_lastBtnState == HIGH && currentState == LOW) {
    if (millis() - _lastBtnPressTime > 50) {
      _lastBtnPressTime = millis();
      _isLongPressHandled = false;
      _lastActivityTime = millis();
      wakeupOLED();
    }
  }

  if (currentState == LOW && !_isLongPressHandled) {
    if (millis() - _lastBtnPressTime >= 800) {
      event = ENC_BTN_LONG_PRESS;
      _isLongPressHandled = true;
    }
  }

  if (_lastBtnState == LOW && currentState == HIGH) {
    if (!_isLongPressHandled && (millis() - _lastBtnPressTime > 50)) {
      event = ENC_BTN_CLICKED;
    }
    _lastBtnPressTime = millis();
  }

  _lastBtnState = currentState;
  return event;
}

void MiniMicroKit_PTL::setLED(bool red, bool yellow, bool green) {
  _ledBlinkActive = false;
  // Active-HIGH (true = HIGH = ติด / false = LOW = ดับ)
  digitalWrite(PTL_LED_RED, red ? HIGH : LOW);
  digitalWrite(PTL_LED_YELLOW, yellow ? HIGH : LOW);
  digitalWrite(PTL_LED_GREEN, green ? HIGH : LOW);
}

void MiniMicroKit_PTL::beep(unsigned int frequency, unsigned long durationMs) {
  tone(PTL_BUZZER, frequency, durationMs);
}

void MiniMicroKit_PTL::playSystemSound(SoundType type) {
  switch (type) {
    case SOUND_CLICK:         beep(1500, 30); break;
    case SOUND_SUCCESS:       tone(PTL_BUZZER, 1000, 50); delay(60); tone(PTL_BUZZER, 2000, 80); break;
    case SOUND_ERROR:         tone(PTL_BUZZER, 400, 150); delay(180); tone(PTL_BUZZER, 300, 200); break;
    case SOUND_STARTUP:       tone(PTL_BUZZER, 523, 80); delay(90); tone(PTL_BUZZER, 659, 80); delay(90); tone(PTL_BUZZER, 784, 120); break;
  
    case SOUND_CONFIRM:       tone(PTL_BUZZER, 1200, 40); delay(60); tone(PTL_BUZZER, 1200, 40); break; 
    case SOUND_CANCEL:        tone(PTL_BUZZER, 300, 120); break;  
    case SOUND_WARNING:       tone(PTL_BUZZER, 800, 100); delay(150); tone(PTL_BUZZER, 800, 100); break; 
    case SOUND_ALARM:         for(int i=0; i<4; i++) { tone(PTL_BUZZER, 2000, 60); delay(100); } break;
    case SOUND_NOTIFY:        tone(PTL_BUZZER, 1200, 40); delay(80); tone(PTL_BUZZER, 1800, 100); break;
    case SOUND_LEVEL_UP:      tone(PTL_BUZZER, 600, 40); delay(50); tone(PTL_BUZZER, 800, 40); delay(50); tone(PTL_BUZZER, 1200, 120); break;
    case SOUND_POWER_OFF:     tone(PTL_BUZZER, 800, 80); delay(100); tone(PTL_BUZZER, 600, 80); delay(100); tone(PTL_BUZZER, 400, 150); break; 
    case SOUND_BEEP_LONG:     tone(PTL_BUZZER, 1000, 600); break; 
    case SOUND_DOUBLE_CLICK:  tone(PTL_BUZZER, 1800, 25); delay(45); tone(PTL_BUZZER, 1800, 25); break; 
    case SOUND_HAPPY:         tone(PTL_BUZZER, 1000, 50); delay(60); tone(PTL_BUZZER, 1200, 50); delay(60); tone(PTL_BUZZER, 1500, 150); break;
  
    case SOUND_CONNECT:       tone(PTL_BUZZER, 880, 50); delay(60); tone(PTL_BUZZER, 1175, 50); delay(60); tone(PTL_BUZZER, 1760, 100); break;
    case SOUND_DISCONNECT:    tone(PTL_BUZZER, 1000, 80); delay(90); tone(PTL_BUZZER, 500, 150); break; 
    case SOUND_LASER:         for(int f = 2000; f > 400; f -= 200) { tone(PTL_BUZZER, f, 15); delay(15); }break;    
    case SOUND_COIN:          tone(PTL_BUZZER, 988, 80); delay(90); tone(PTL_BUZZER, 1318, 200); break;
    case SOUND_HEARTBEAT:     tone(PTL_BUZZER, 300, 40); delay(80); tone(PTL_BUZZER, 250, 40); break;  
  }
}

void MiniMicroKit_PTL::update() {
  if (_autoSleepEnabled && _oledPowered && (millis() - _lastActivityTime > _autoSleepTimeout)) {
    display.ssd1306_command(SSD1306_DISPLAYOFF);
    _oledPowered = false;
  }

  if (_asyncBeepActive && (millis() - _asyncBeepTimer >= 50)) {
    noTone(PTL_BUZZER);
    _asyncBeepActive = false;
  }

  // Async LED Blink (Active-HIGH)
  if (_ledBlinkActive && (millis() - _ledBlinkTimer >= _ledBlinkInterval)) {
    _ledBlinkTimer = millis();
    _ledBlinkState = !_ledBlinkState;
    digitalWrite(PTL_LED_RED, _ledBlinkState ? HIGH : LOW);
    digitalWrite(PTL_LED_YELLOW, _ledBlinkState ? HIGH : LOW);
    digitalWrite(PTL_LED_GREEN, _ledBlinkState ? HIGH : LOW);
  }
}

bool MiniMicroKit_PTL::initOLEDAuto() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      _oledPowered = false;
      return false;
    }
  }
  _oledPowered = true;
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();
  return true;
}

void MiniMicroKit_PTL::wakeupOLED() {
  if (!_oledPowered) {
    display.ssd1306_command(SSD1306_DISPLAYON);
    _oledPowered = true;
  }
}

void MiniMicroKit_PTL::clearDisplay() { if (_oledPowered) display.clearDisplay(); }
void MiniMicroKit_PTL::displayUpdate() { if (_oledPowered) display.display(); }
void MiniMicroKit_PTL::printText(String text, int x, int y, int size) {
  if (!_oledPowered) return;
  display.setTextSize(size);
  display.setCursor(x, y);
  display.print(text);
}
void MiniMicroKit_PTL::drawProgressBar(int x, int y, int w, int h, int pct) {
  if (!_oledPowered) return;
  display.drawRect(x, y, w, h, SSD1306_WHITE);
  int fillW = map(constrain(pct, 0, 100), 0, 100, 0, w - 4);
  if (fillW > 0) display.fillRect(x + 2, y + 2, fillW, h - 4, SSD1306_WHITE);
}
void MiniMicroKit_PTL::showDialog(String title, String msg, unsigned long displayMs) {
  if (!_oledPowered) return;
  display.clearDisplay();
  display.drawRect(5, 5, 118, 54, SSD1306_WHITE);
  printText(title, 12, 12, 1);
  display.drawLine(10, 24, 118, 24, SSD1306_WHITE);
  printText(msg, 12, 32, 1);
  display.display();
  delay(displayMs);
}
void MiniMicroKit_PTL::setAutoSleep(bool enable, unsigned long timeoutMs) {
  _autoSleepEnabled = enable;
  _autoSleepTimeout = timeoutMs;
}
bool MiniMicroKit_PTL::isOLEDPowered() { return _oledPowered; }
int MiniMicroKit_PTL::readAnalogSmooth(uint8_t pin, float alpha) {
  static float smoothVal = 0;
  smoothVal = (alpha * analogRead(pin)) + ((1.0f - alpha) * smoothVal);
  return (int)smoothVal;
}
bool MiniMicroKit_PTL::every(unsigned long &timer, unsigned long intervalMs) {
  if (millis() - timer >= intervalMs) {
    timer = millis();
    return true;
  }
  return false;
}
void MiniMicroKit_PTL::scanI2CBus(bool printToSerial) {
  if (!printToSerial) return;
  Serial.println("--- Scanning I2C Bus (SDA:2, SCL:1) ---");
  byte count = 0;
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Device found at 0x");
      if (i < 16) Serial.print("0");
      Serial.println(i, HEX);
      count++;
    }
  }
  Serial.println(count == 0 ? "No I2C devices found!" : "Scan Complete.");
}
void MiniMicroKit_PTL::beepAsync(unsigned long durationMs) {
  tone(PTL_BUZZER, 2000);
  _asyncBeepTimer = millis();
  _asyncBeepActive = true;
}
void MiniMicroKit_PTL::blinkLED(bool red, bool yellow, bool green, unsigned long intervalMs) {
  _ledBlinkActive = true;
  _ledBlinkInterval = intervalMs;
  _ledBlinkTimer = millis();
}