#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>
#include <BH1750.h>
#include <Wire.h>

// Định nghĩa cấu trúc dữ liệu
typedef struct struct_message {
    float temp;
    float hum;
    float light;
} struct_message;

struct_message sendData;
struct_message recvData;

// IP và MAC giả định để nộp bài
uint8_t peerAddress[] = {0x24, 0x6F, 0x28, 0xAE, 0xD5, 0x30}; 

#define DHTPIN 4
DHT dht(DHTPIN, DHT11);
BH1750 lightMeter;

// Hàm nhận dữ liệu
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&recvData, incomingData, sizeof(recvData));
}

// Hàm gửi dữ liệu
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
    Serial.begin(115200);
    
    // Khởi tạo cảm biến
    dht.begin();
    Wire.begin();
    lightMeter.begin();

    // Khởi tạo Wi-Fi và ESP-NOW
    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) return;

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    
    // Đăng ký Peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
}

void loop() {
    // Đọc dữ liệu giả lập/thực tế
    sendData.temp = dht.readTemperature();
    sendData.hum = dht.readHumidity();
    sendData.light = lightMeter.readLightLevel();

    // In ra Serial để kiểm tra
    Serial.printf("T: %.2fC | H: %.2f%% | L: %.2f lux\n", sendData.temp, sendData.hum, sendData.light);
    
    // Gửi qua ESP-NOW
    esp_now_send(peerAddress, (uint8_t *) &sendData, sizeof(sendData));
    
    delay(2000);
}