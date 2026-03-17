#include <WiFi.h>
#include <ESP32MQTTClient.h>
#include <Wire.h>
#include "MPU6050_light.h"

const char* ssid = "VodafoneNet-9TVVZM";
const char* password = "22ZCpeJ5th54hbbX";

const char* mqttServer = "c58b49f285464419a6a43e78edb87899.s1.eu.hivemq.cloud";
const int   mqttPort   = 8883;
const char* mqttUser   = "deprem";
const char* mqttPass   = "Deprem123";
const char* mqttTopic  = "deprem/tespit";

ESP32MQTTClient mqttClient;
MPU6050 mpu(Wire);

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (mpu.begin() != 0) {
    Serial.println("MPU6050 başlatma hatası!");
    while (1);
  }

  WiFi.begin(ssid, password);
  Serial.print("WiFi bağlanıyor");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" WiFi bağlı!");

  // MQTT bağlantısı (URI + username + password)
  mqttClient.setURI(mqttServer, mqttUser, mqttPass);
  mqttClient.enableLastWillMessage("deprem/status", "offline", 1, true);
  mqttClient.startMqtt();
}

void loop() {
  mpu.update();
  float x = mpu.getAccX();
  float y = mpu.getAccY();
  float z = mpu.getAccZ();

  // JSON string std::string tipinde olmalı
  std::string payload = 
      std::string("{\"x\":") + std::to_string(x) +
      ",\"y\":" + std::to_string(y) +
      ",\"z\":" + std::to_string(z) + "}";

  mqttClient.publish(mqttTopic, payload);
  Serial.println(("Gönderildi: " + payload).c_str());

  delay(1000);
}
