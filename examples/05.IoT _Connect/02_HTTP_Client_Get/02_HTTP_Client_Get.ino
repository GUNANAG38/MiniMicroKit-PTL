/*
  MiniMicroKit_PTL - HTTP Client (REST API GET Request)
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// เปลี่ยน SSID และ Password ให้ตรงกับ WiFi ที่ต้องการเชื่อมต่อ
const char* ssid = "xxxxxx";
const char* password = "xxxxx";

// API สำหรับทดสอบดึงข้อมูล (ใช้ HTTP ปกติเพื่อความง่ายในการเริ่มต้น)
const char* serverUrl = "http://ip-api.com/json/";

void setup() {
  Serial.begin(115200);
  kit.begin();

  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("WiFi Connecting...", 0, 20, 1);
    kit.displayUpdate();
  }

  // เริ่มต้นเชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("WiFi Connected!", 0, 20, 1);
    kit.displayUpdate();
    delay(1500);
  }
}

void loop() {
  kit.update();

  // ตรวจสอบสถานะการเชื่อมต่อ WiFi ก่อนส่งคำขอ
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    Serial.println("Sending HTTP GET request...");
    
    // กำหนด URL ที่ต้องการเชื่อมต่อ
    http.begin(serverUrl);

    // ส่งคำขอแบบ GET ไปยังเซิร์ฟเวอร์
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      // รับข้อมูล Response กลับมาเป็นข้อความ (Payload)
      String payload = http.getString();
      Serial.println("--- Response Payload ---");
      Serial.println(payload);
      Serial.println("------------------------");

      // แสดงผลสถานะสำเร็จบนจอ OLED
      if (kit.isOLEDPowered()) {
        kit.clearDisplay();
        kit.printText("HTTP GET OK", 0, 0, 1);
        kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
        kit.printText("Status: " + String(httpResponseCode), 0, 20, 1);
        kit.printText("Data Received!", 0, 35, 1);
        kit.displayUpdate();
      }
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      
      if (kit.isOLEDPowered()) {
        kit.clearDisplay();
        kit.printText("HTTP Error!", 0, 20, 1);
        kit.displayUpdate();
      }
    }

    // ปิดการเชื่อมต่อ HTTP
    http.end();
  }

  // หน่วงเวลา 15 วินาทีก่อนทำการส่งคำขอรอบถัดไป (ป้องกันการยิงถี่เกินไปจนเซิร์ฟเวอร์บล็อก)
  delay(15000);
}
