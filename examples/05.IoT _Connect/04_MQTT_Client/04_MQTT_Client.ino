/*
  MiniMicroKit_PTL - MQTT Client (Publish/Subscribe)
  Developer: GelectronLAB
  Board: MiniMicroKit PT-L V1.0 (ESP32-C3)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h> //ต้องติดตั้ง PubSubClient (Nick O'Leary)
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// เปลี่ยน SSID และ Password ให้ตรงกับ WiFi ที่ต้องการเชื่อมต่อ
const char* ssid = "xxxxxx";
const char* password = "xxxxx";

// ใช้ Public MQTT Broker สำหรับทดสอบ
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// กำหนด Topic สำหรับรับส่งข้อมูล (ควรเปลี่ยนชื่อ Topic ไม่ให้ซ้ำกัน)
const char* topic_pub = "gelectron/ptl/sensor";
const char* topic_sub = "gelectron/ptl/control";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
int counter = 0;

// ฟังก์ชันรับข้อความเมื่อมีคนส่งมายัง Topic ที่เรา Subscribe ไว้
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // นำข้อความที่ได้รับมาแสดงบนจอ OLED
  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("MQTT RECEIVED", 0, 0, 1);
    kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
    kit.printText("Msg: " + message, 0, 20, 1);
    kit.displayUpdate();
  }
}

// ฟังก์ชันเชื่อมต่อ MQTT Broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // สร้าง Client ID สุ่มเพื่อไม่ให้ซ้ำ
    String clientId = "PTL-ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // เมื่อเชื่อมต่อสำเร็จ ให้ทำการ Subscribe รอรับคำสั่ง
      client.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  kit.begin();

  // เชื่อมต่อ WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");

  // ตั้งค่า MQTT Broker
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  if (kit.isOLEDPowered()) {
    kit.clearDisplay();
    kit.printText("MQTT Ready", 0, 20, 1);
    kit.displayUpdate();
    delay(1000);
  }
}

void loop() {
  kit.update();

  // ตรวจสอบการเชื่อมต่อ MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // ส่งข้อมูล (Publish) ทุกๆ 5 วินาที
  unsigned long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    counter++;

    String payload = "Hello IoT #" + String(counter);
    Serial.print("Publish message: ");
    Serial.println(payload);

    // ส่งข้อมูลไปยัง Broker
    client.publish(topic_pub, payload.c_str());

    if (kit.isOLEDPowered()) {
      kit.clearDisplay();
      kit.printText("MQTT PUBLISH", 0, 0, 1);
      kit.display.drawLine(0, 10, 127, 10, SSD1306_WHITE);
      kit.printText("Sent: #" + String(counter), 0, 20, 1);
      kit.displayUpdate();
    }
  }
}