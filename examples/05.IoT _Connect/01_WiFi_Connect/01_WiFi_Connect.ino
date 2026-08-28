/*
  MiniMicroKit_PTL - WiFi Connection Basic (IP Address)
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <WiFi.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// เปลี่ยน SSID และ Password ให้ตรงกับ WiFi ที่ต้องการเชื่อมต่อ
const char* ssid = "xxxxxx";
const char* password = "xxxxx";

void setup() {
  Serial.begin(115200);
  kit.begin();

  delay(1000);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // เริ่มต้นคำสั่งเชื่อมต่อ Wi-Fi
  WiFi.begin(ssid, password);

  // เช็คสถานะการเชื่อมต่อ (รอจนกว่าจะเชื่อมต่อสำเร็จ)
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempt++;
    
    // หากพยายามเกิน 20 ครั้ง (10 วินาที) แล้วยังไม่เชื่อมต่อ ให้แจ้งเตือน
    if (attempt > 20) {
      Serial.println("\nFailed to connect to Wi-Fi. Please check SSID/Password.");
      break;
    }
  }

  // หากเชื่อมต่อสำเร็จ
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected successfully!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP()); // แสดงหมายเลข IP ของบอร์ด
  }
}

void loop() {
  kit.update();
  
  // ในโหมดนี้ ให้ตรวจสอบสถานะการเชื่อมต่อผ่าน Serial Monitor
  delay(2000);
}