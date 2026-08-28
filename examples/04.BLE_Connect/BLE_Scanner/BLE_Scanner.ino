/*
  MiniMicroKit_PTL - BLE Professional Scanner
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

int scanTime = 5;
const int MAX_DEVICES = 10;
BLEScan* pBLEScan;

String devNames[MAX_DEVICES];
int devRSSIs[MAX_DEVICES];
int totalDevices = 0;
int selectedIndex = 0;

void setup() {
  Serial.begin(115200);
  kit.begin();
  
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(500);
  pBLEScan->setWindow(450);

  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("BLE SCANNER", 0, 0, 1);
    kit.display.drawLine(0, 9, 127, 9, SSD1306_WHITE);
    kit.printText("Initializing...", 0, 25, 1);
    kit.displayUpdate();
  }
}

void loop() {
  kit.update();

  pBLEScan->clearResults();
  BLEScanResults* foundDevices = pBLEScan->start(scanTime, false);
  int count = foundDevices->getCount();
  int foundCount = 0;

  for (int i = 0; i < count && foundCount < MAX_DEVICES; i++) {
    BLEAdvertisedDevice device = foundDevices->getDevice(i);
    String name = device.getName();
    
    if (name.length() > 0) {
      // ตัดข้อความ เอาเฉพาะคำแรกก่อนเจอช่องว่าง (Space)
      int spaceIdx = name.indexOf(' ');
      if (spaceIdx != -1) {
        name = name.substring(0, spaceIdx);
      }

      // ตรวจสอบชื่อซ้ำ
      bool isDuplicate = false;
      for(int j = 0; j < foundCount; j++) {
        if(devNames[j] == name) isDuplicate = true;
      }
      if(!isDuplicate) {
        devNames[foundCount] = name;
        devRSSIs[foundCount] = device.getRSSI();
        foundCount++;
      }
    }
  }
  
  totalDevices = (foundCount > 0) ? foundCount : 1;
  if(foundCount == 0) {
    devNames[0] = "Searching";
    devRSSIs[0] = 0;
  }

  // หมุนเลื่อนเลือกรายการด้วย Rotary Encoder
  selectedIndex = kit.updateMenuIndex(selectedIndex, 0, totalDevices - 1, false);

  // แสดงผลหน้าจอ OLED
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    
    // Header (100% width)
    kit.printText("BLE SCANNER", 0, 0, 1);
    kit.printText(String(selectedIndex + 1) + "/" + String(totalDevices), 98, 0, 1);
    kit.display.drawLine(0, 9, 127, 9, SSD1306_WHITE);

    // ฝั่งซ้าย 50% แสดงชื่อคำแรก (จำกัดความยาวไม่ให้เกิน 10 ตัวอักษร)
    String displayName = devNames[selectedIndex];
    if (displayName.length() > 10) {
      displayName = displayName.substring(0, 10);
    }
    kit.printText(displayName, 15, 30, 2);

    // เส้นแบ่งกลางจอ (แนวตั้งที่ตำแหน่ง 64)
    kit.display.drawLine(64, 10, 64, 63, SSD1306_WHITE);

    // ฝั่งขวา 50% แสดง RSSI
    kit.printText("RSSI", 88, 20, 1);
    kit.printText(String(devRSSIs[selectedIndex]), 80, 32, 2); // ค่า RSSI ขนาดตัวอักษร 2
    kit.printText("dBm", 90, 53, 1);

    kit.displayUpdate();
  }
}