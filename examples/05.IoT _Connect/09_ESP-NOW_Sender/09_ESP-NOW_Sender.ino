/*
  MiniMicroKit_PTL - ESP-NOW Sender
  Developer : GelectronLAB
  Board : MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <esp_now.h>
#include <WiFi.h>

// แทนที่ด้วย MAC Address ของบอร์ด Receiver (ตัวรับ)
uint8_t broadcastAddress[] = {0x94, 0xA9, 0x90, 0x96, 0x2B, 0xB8};

// โครงสร้างข้อมูลสำหรับส่งข้อความ
typedef struct struct_message {
    char text[32];
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // เริ่มต้นระบบ ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // ลงทะเบียนบอร์ดปลายทาง (Peer)
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // กำหนดข้อความที่ต้องการส่ง
  strcpy(myData.text, "I LOVE MY JOB");
  
  // ส่งข้อมูลผ่าน ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  
  if (result == ESP_OK) {
    Serial.println("Data sent successfully: I LOVE MY JOB");
  } else {
    Serial.println("Error sending the data");
  }
  
  delay(2000); // ส่งข้อความทุกๆ 2 วินาที
}