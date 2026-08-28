/*
  MiniMicroKit_PTL - JSON Parsing from REST API
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// เปลี่ยน SSID และ Password ให้ตรงกับ WiFi ที่ต้องการเชื่อมต่อ
const char* ssid = "xxxxxx";
const char* password = "xxxxx";
const char* serverUrl = "http://ip-api.com/json/";

void setup() {
  Serial.begin(115200);
  kit.begin();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
}

void loop() {
  kit.update();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Raw JSON received.");

      // สร้าง Document สำหรับแกะ JSON (กำหนดขนาดหน่วยความจำตามความเหมาะสม)
      JsonDocument doc; 
      
      // แปลงข้อความ Payload ให้เป็น JSON Object
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
      }

      // ดึงค่าเฉพาะคีย์ที่ต้องการ
      String ip = doc["query"];
      String country = doc["country"];
      String city = doc["city"];

      // แสดงผลผ่าน Serial Monitor
      Serial.println("--- Parsed Data ---");
      Serial.println("IP: " + ip);
      Serial.println("Country: " + country);
      Serial.println("City: " + city);
      Serial.println("-------------------");

      // แสดงผลลงหน้าจอ OLED
      if (kit.isOLEDPowered()) {
        kit.clearDisplay();
        kit.printText("JSON PARSED", 0, 0, 1);
        kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
        
        kit.printText("IP: " + ip, 0, 16, 1);
        kit.printText("Cty: " + country, 0, 28, 1);
        kit.printText("City: " + city, 0, 40, 1);
        
        kit.displayUpdate();
      }
    } else {
      Serial.println("HTTP Error");
    }
    http.end();
  }

  delay(20000); // อัปเดตทุก 20 วินาที
}