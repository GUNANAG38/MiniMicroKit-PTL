/*
  MiniMicroKit_PTL - Over-The-Air (OTA) Firmware Update (Fixed Error Constants)
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// เปลี่ยน SSID และ Password ให้ตรงกับ WiFi ที่ต้องการเชื่อมต่อ
const char* ssid = "xxxxxx";
const char* password = "xxxxx";

void setup() {
  Serial.begin(115200);
  kit.begin();

  // ตั้งค่าโหมด WiFi เป็น Station
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // รอการเชื่อมต่อ WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ตั้งชื่อโฮสต์สำหรับตรวจจับบอร์ดผ่านเครือข่าย (mDNS)
  ArduinoOTA.setHostname("MiniMicroKit-PTL");

  // เมื่อเริ่มกระบวนการอัปเดต
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS (Filesystem)
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
    
    if (kit.isOLEDPowered()) {
      kit.clearDisplay();
      kit.printText("OTA UPDATE", 0, 0, 1);
      kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
      kit.printText("Updating...", 0, 20, 1);
      kit.displayUpdate();
    }
  });

  // เมื่ออัปเดตเสร็จสิ้น
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
    if (kit.isOLEDPowered()) {
      kit.clearDisplay();
      kit.printText("OTA UPDATE", 0, 0, 1);
      kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
      kit.printText("Update Success!", 0, 20, 1);
      kit.displayUpdate();
    }
  });

  // แสดงความคืบหน้าการอัปเดตทาง Serial Monitor
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %3u%%\r", (progress / (total / 100)));
  });

  // จัดการกรณีเกิดข้อผิดพลาด (ปรับใช้ชื่อนำหน้าด้วย OTA_ ตามมาตรฐาน Core ใหม่)
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  // สั่งเริ่มต้นระบบ OTA
  ArduinoOTA.begin();
  Serial.println("OTA Ready");

  // แสดงผลสถานะพร้อมใช้งานบนจอ OLED
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("IOT SYSTEM", 0, 0, 1);
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    kit.printText("OTA Ready", 0, 20, 1); //TESTING
    kit.printText(WiFi.localIP().toString(), 0, 35, 1);
    kit.displayUpdate();
  }
}

void loop() {
  ArduinoOTA.handle();
  kit.update();
}