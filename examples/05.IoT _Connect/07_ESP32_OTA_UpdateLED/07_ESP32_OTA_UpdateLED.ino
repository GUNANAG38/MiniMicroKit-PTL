/*
  MiniMicroKit_PTL - OTA Update + Library LED Cycling Example
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

// ตัวแปรสำหรับจับเวลาสลับไฟ LED แบบ Non-blocking
unsigned long ledTimer = 0;
int ledStateStep = 0;

void setup() {
  Serial.begin(115200);
  kit.begin();

  // เชื่อมต่อ Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // ตั้งค่าระบบ OTA
  ArduinoOTA.setHostname("MiniMicroKit-PTL");
  ArduinoOTA.setPassword("admin"); // กำหนดรหัสผ่านเป็นค่าว่างเพื่อความสะดวกในการอัปเดตผ่าน WiFi

  // Event ต่างๆ ของระบบ OTA
  ArduinoOTA.onStart([]() {
    Serial.println("Start updating sketch...");
    if (kit.isOLEDPowered()) {
      kit.clearDisplay();
      kit.printText("OTA UPDATE", 0, 0, 1);
      kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
      kit.printText("Updating...", 0, 20, 1);
      kit.displayUpdate();
    }
  });

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

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %3u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
  Serial.println("OTA Ready");

  // แสดงผลสถานะบนจอ OLED
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("IOT SYSTEM", 0, 0, 1);
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    kit.printText("OTA/LED Ready", 0, 20, 1);
    kit.printText(WiFi.localIP().toString(), 0, 35, 1);
    kit.displayUpdate();
  }
}

void loop() {
  // จำเป็นต้องเรียกใช้งานทั้งคู่อยู่ใน loop() เสมอเพื่อให้ระบบพื้นหลังทำงานสมบูรณ์
  ArduinoOTA.handle();
  kit.update();

  // สลับไฟ LED วิ่งวน (แดง > เหลือง > เขียว > ดับ) ทุกๆ 500ms โดยใช้ kit.every()
  if (kit.every(ledTimer, 500)) {
    switch (ledStateStep) {
      case 0:
        kit.setLED(true, false, false);  // เปิดไฟสีแดง (เหลือง/เขียว ดับ)
        break;
      case 1:
        kit.setLED(false, true, false);  // เปิดไฟสีเหลือง (แดง/เขียว ดับ)
        break;
      case 2:
        kit.setLED(false, false, true);  // เปิดไฟสีเขียว (แดง/เหลือง ดับ)
        break;
      case 3:
        kit.setLED(false, false, false); // ดับทุกดวง
        break;
    }
    ledStateStep = (ledStateStep + 1) % 4; // วนรอบค่า 0 ถึง 3
  }
}