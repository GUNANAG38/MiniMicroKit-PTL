/*
  MiniMicroKit_PTL - ESP-NOW_P2P Chat
  Developer: GelectronLAB
*/

#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;
char msgBuf[64];

// นำ MAC Address ของคู่สนทนามาแทนในปีกกา
uint8_t peerMac[] = {0x94, 0xA9, 0x90, 0x96, 0x2B, 0xB8};   // MMK to test
//uint8_t peerMac[] = {0x44, 0xBD, 0x8D, 0x24, 0xCB, 0x24}; // test to MMK

String hist[30]; int total = 0, scroll = 0;

void addChat(String pre, String txt) {
  String line = pre + txt;
  while (line.length() > 0) {
    if (total < 30) hist[total++] = line.substring(0, 21);
    line = line.length() > 21 ? (pre.startsWith("Me") ? "     " : "      ") + line.substring(21) : "";
  }
  scroll = max(0, total - 5);
}

void drawOLED() {
  kit.clearDisplay(); 
  kit.printText("MiniMicro CHAT", 25, 0, 1); 
  kit.display.drawLine(0, 9, 127, 9, 1);
  for (int i = 0, y = 12; i < 5; i++, y += 10) 
    if (scroll + i < total) kit.printText(hist[scroll + i].c_str(), 0, y, 1);
  kit.displayUpdate();
}

void onRecv(const esp_now_recv_info_t *info, const uint8_t *d, int len) {
  // ถ้าข้อความไม่ได้ส่งมาจาก MAC ของคู่สนทนาตัวจริง ให้ข้ามทันที (ป้องกันคนอื่นแทรก)
  if (memcmp(info->src_addr, peerMac, 6) != 0) return;

  memcpy(msgBuf, d, sizeof(msgBuf)); 
  addChat("Peer:", msgBuf); 
  drawOLED();
}

void setup() {
  Serial.begin(115200); kit.begin(); 
  WiFi.mode(WIFI_STA); 

  // แสดง MAC Address ของบอร์ดตัวเองออก Serial Monitor เพื่อให้คุณนำไปใส่อีกเครื่อง
  Serial.print("My MAC Address: ");
  Serial.println(WiFi.macAddress());

  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
  esp_now_init(); 
  esp_now_register_recv_cb(onRecv);
  
  esp_now_peer_info_t p = {}; 
  memcpy(p.peer_addr, peerMac, 6); 
  p.channel = 1; 
  p.encrypt = false;
  esp_now_add_peer(&p);
  
  addChat("", "Ready."); drawOLED();
}

void loop() {
  kit.update();
  
  if (Serial.available()) {
    String in = Serial.readStringUntil('\n'); in.trim();
    if (in.length()) { 
      addChat("Me:  ", in); 
      in.toCharArray(msgBuf, 64); 
      // ส่งข้อมูลตรงไปยัง MAC Address ของคู่สนทนาเท่านั้น
      esp_now_send(peerMac, (uint8_t*)&msgBuf, sizeof(msgBuf)); 
      drawOLED(); 
    }
  }
  
  int s = kit.updateMenuIndex(scroll, 0, max(0, total - 5), false);
  if (s != scroll) { scroll = s; drawOLED(); }
}