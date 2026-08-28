/*
  MiniMicroKit_PTL - ESP-NOW Receiver and OLED Centered Text (Size 1)
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <esp_now.h>
#include <WiFi.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// โครงสร้างข้อมูลสำหรับรับข้อความ (ต้องตรงกับฝั่ง Sender)
typedef struct struct_message {
    char text[32];
} struct_message;

struct_message myData;

// ฟังก์ชันดักจับเมื่อมีข้อมูลส่งเข้ามาผ่าน ESP-NOW
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Received Text: ");
  Serial.println(myData.text);

  // แสดงผลลงหน้าจอ OLED เมื่อได้รับข้อมูล
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    
    // คำนวณตำแหน่งกึ่งกลางจอ (OLED ขนาด 128x64 พิกเซล) สำหรับขนาดตัวอักษร 1
    // ตัวอักษรขนาด 1 กว้างประมาณ 6 พิกเซล และสูงประมาณ 8 พิกเซล
    int charWidth = 6; 
    int textLength = strlen(myData.text);
    int totalWidth = textLength * charWidth;
    int x = (128 - totalWidth) / 2; // คำนวณกึ่งกลางแนวนอน
    int y = (64 - 8) / 2;           // คำนวณกึ่งกลางแนวตั้ง ((64 - 8) / 2 = 28)

    // สั่งพิมพ์ข้อความขนาด 1 ตามพิกัดที่คำนวณได้
    kit.printText(myData.text, x, y, 1);
    kit.displayUpdate();
  }
}

void setup() {
  Serial.begin(115200);
  kit.begin();

  // ตั้งค่า WiFi เป็น Station Mode สำหรับใช้งาน ESP-NOW
  WiFi.mode(WIFI_STA);

  // เริ่มต้นระบบ ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // ลงทะเบียน Callback ฟังก์ชันสำหรับรับข้อมูล
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  // แสดงสถานะเริ่มต้นบนจอ OLED
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("ESP-NOW Ready", 0, 0, 1);
    kit.printText("Waiting message...", 0, 20, 1);
    kit.displayUpdate();
  }
}

void loop() {
  // ต้องเรียก kit.update() ใน loop() เสมอเพื่อให้ระบบทำงานสมบูรณ์
  kit.update();
}