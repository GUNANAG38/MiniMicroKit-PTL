/*
  MiniMicroKit_PTL - Independent 3 LEDs Web Bluetooth Control
  Developer: GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a1"

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("Received: " + value);

        // ควบคุม LED สีแดง (Red)
        if (value == "RED_ON")      digitalWrite(PTL_LED_RED, HIGH);
        else if (value == "RED_OFF") digitalWrite(PTL_LED_RED, LOW);

        // ควบคุม LED สีเหลือง (Yellow)
        else if (value == "YELLOW_ON")      digitalWrite(PTL_LED_YELLOW, HIGH);
        else if (value == "YELLOW_OFF") digitalWrite(PTL_LED_YELLOW, LOW);

        // ควบคุม LED สีเขียว (Green)
        else if (value == "GREEN_ON")      digitalWrite(PTL_LED_GREEN, HIGH);
        else if (value == "GREEN_OFF") digitalWrite(PTL_LED_GREEN, LOW);
      }
    }
};

void setup() {
  Serial.begin(115200);
  kit.begin();

  // กำหนดพิน LED ทั้ง 3 ดวงเป็นเอาต์พุต
  pinMode(PTL_LED_RED, OUTPUT);
  pinMode(PTL_LED_YELLOW, OUTPUT);
  pinMode(PTL_LED_GREEN, OUTPUT);

  // เริ่มต้นระบบ BLE ตั้งชื่อบอร์ดว่า "MicroKit-BLE"
  BLEDevice::init("MicroKit-BLE");
  BLEServer *pServer = BLEDevice::createServer();
  
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID,BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEDevice::getAdvertising()->start();

  // แสดงผลบนหน้าจอ OLED
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("MicroKit-BLE", 0, 0, 1);
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    kit.printText("Name: MicroKit-BLE", 0, 20, 1);
    kit.printText("Independent Control", 0, 35, 1);
    kit.displayUpdate();
  }
}

void loop() {
  kit.update();
}