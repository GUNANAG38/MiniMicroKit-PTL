/*
  MiniMicroKit_PTL - ESP32 Local Web Server (Working Hardware LED Control)
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// เปลี่ยน SSID และ Password ให้ตรงกับ WiFi ที่ต้องการเชื่อมต่อ
const char* ssid = "xxxxxx";
const char* password = "xxxxx";

// กำหนดขาพินสำหรับควบคุม LED (ESP32-C3 ใช้ GPIO 5)
const int ledPin = 5; 

// สร้าง Web Server ที่พอร์ต 80
WebServer server(80);

// ตัวแปรสถานะ LED
bool ledState = false;

// ฟังก์ชันสร้างหน้าเว็บ HTML
void handleRoot() {
  String html = "<!DOCTYPE html>";
  html += "<html><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>GelectronLAB IoT</title>";
  html += "<style>";
  html += "body { font-family: Arial; text-align: center; margin-top: 50px; background-color: #f4f4f9; }";
  html += "h1 { color: #333; }";
  html += ".btn { background-color: #4CAF50; color: white; padding: 15px 32px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 8px; border: none;}";
  html += ".btn-off { background-color: #f44336; }";
  html += "</style></head><body>";
  html += "<h1>MiniMicroKit PT-L</h1>";
  html += "<h1>Web Server</h1>";
  html += "<p>Developer: GelectronLAB</p>";
  
  if (ledState) {
    html += "<p>LED Status: <b style='color:green;'>ON</b></p>";
    html += "<a href=\"/toggle\"><button class=\"btn btn-off\">Turn OFF</button></a>";
  } else {
    html += "<p>LED Status: <b style='color:red;'>OFF</b></p>";
    html += "<a href=\"/toggle\"><button class=\"btn\">Turn ON</button></a>";
  }
  
  html += "</body></html>";

  server.send(200, "text/html", html);
}

// ฟังก์ชันเมื่อมีการกดเปลี่ยนสถานะบนเว็บ
void handleToggle() {
  ledState = !ledState; // สลับสถานะ
  
  // สั่งงานเปิด-ปิดหลอดไฟ
  digitalWrite(ledPin, ledState ? HIGH : LOW);

  // รีเฟรชหน้าเว็บกลับมาหน้าหลัก
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  kit.begin();

  // กำหนดโหมดพิน LED เป็น OUTPUT
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // เริ่มต้นให้ดับไว้ก่อน

  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address for Web Server: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);

  server.begin();
  Serial.println("HTTP server started");

  // แสดงผลหน้าจอ OLED
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    
    // หัวข้อด้านบนสุด
    kit.printText("IOT SYSTEM Web Server", 0, 0, 1);
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    
    // แสดง IP Address บรรทัดล่าง
    kit.printText("IP", 52, 25, 2);
    kit.printText(WiFi.localIP().toString(), 28, 50, 1);
    
    kit.displayUpdate();
  }
}

void loop() {
  kit.update();
  server.handleClient();
}