#include <WiFi.h>
#include <PubSubClient.h>
#include "MiniMicroKit_PTL.h"

MiniMicroKit_PTL kit;

// ตั้งค่า ssid และ password ให้ตรงกับ WiFi ที่ต้องการเชื่อมต่อ
const char* ssid     = "Gunanag_2.4GHz";
const char* password = "0822804250";

const char* mqtt_server = "broker.emqx.io";
const int   mqtt_port   = 1883;

// แก้ไข Topic ให้ตรงกับ Webapp (School ID / Name ID)
const char* mqtt_topic  = "gelectronlab/PTL-School/group1/status"; 

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  kit.clearDisplay();
  kit.printText("AI Status:", 0, 0, 1);

  if (message == "Hongthai") {
      kit.setLED(false, false, true);
      kit.printText("Hongthai", 0, 16, 2);
      kit.printText("Status : OK", 0, 50, 1);
  } 
  else if (message == "Red Marker") {
      kit.playSystemSound(SOUND_ERROR);
      kit.setLED(true, false, false);
      kit.printText("Red Marker", 0, 16, 2); 
      kit.printText("Status : Alert", 0, 50, 1);
  } 
  else if (message == "None" || message == "Unknown" || message == "Uncertain...") {
      kit.setLED(false, true, false);
      kit.printText("No Object", 0, 16, 2);    
      kit.printText("Please Check", 0, 50, 1);
  }
  else if (message == "Test_Class") {
      kit.playSystemSound(SOUND_CONNECT);
      kit.setLED(true, true, true);
      kit.printText("Test Pass", 0, 16, 2);
      kit.printText("Connected!", 0, 50, 1);
  }
  else {
      kit.setLED(true, true, false); 
      kit.printText("Class Name:", 0, 10, 1);
      kit.printText(message, 0, 16, 2);
      kit.printText("Active", 0, 50, 1);
  }
  
  kit.displayUpdate();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // ดึงค่าจาก mqtt_topic มาสร้าง Client ID อัตโนมัติ ป้องกันการชนกันบน Broker สาธารณะ
    String topicStr = String(mqtt_topic);
    int firstSlash = topicStr.indexOf('/');
    int secondSlash = topicStr.indexOf('/', firstSlash + 1);
    int thirdSlash = topicStr.indexOf('/', secondSlash + 1);
    
    String schoolAndGroup = "ESP32-";
    if (secondSlash != -1 && thirdSlash != -1) {
      schoolAndGroup += topicStr.substring(secondSlash + 1, thirdSlash);
      schoolAndGroup.replace('/', '-');
    } else {
      schoolAndGroup += "default";
    }
    
    String clientId = schoolAndGroup + "-" + String(random(0xffff), HEX) + "-" + String(micros());
    
    if (client.connect(clientId.c_str())) {
      Serial.println("connected!");
      client.subscribe(mqtt_topic);
      Serial.print("Subscribed to topic: ");
      Serial.println(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  
  kit.begin();
  kit.showDialog("AI System", "Connecting Wi-Fi", 1000);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected successfully!");
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setBufferSize(512);

  kit.clearDisplay();
  kit.printText("Ready for AI", 0, 0, 1);
  kit.printText("Topic Ready", 0, 20, 1);
  kit.displayUpdate();
}

void loop() {
  kit.update();
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}