#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include <DHT.h>

typedef struct struct_message {
    float temp;
    float hum;
    float light;
} struct_message;

struct_message sendData;
struct_message recvData;

uint8_t peerAddress[] = {0xA4, 0xF0, 0x0F, 0x75, 0x17, 0xCC}; 

#define DHTPIN 4
DHT dht(DHTPIN, DHT11);

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
    memcpy(&recvData, incomingData, sizeof(recvData));
    Serial.printf("Anh sang tu Con B: %.2f lux\n", recvData.light);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    WiFi.mode(WIFI_STA);
    
    if (esp_now_init() != ESP_OK) return;

    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
    
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, peerAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    esp_now_add_peer(&peerInfo);
}

void loop() {
    sendData.temp = dht.readTemperature();
    sendData.hum = dht.readHumidity();
    sendData.light = 0; 

    esp_now_send(peerAddress, (uint8_t *) &sendData, sizeof(sendData));
    delay(2000);
}