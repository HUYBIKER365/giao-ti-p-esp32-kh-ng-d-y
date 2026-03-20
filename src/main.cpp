#include <Arduino.h>
#include <DHT.h>
#include "BluetoothSerial.h"

// Cấu hình chân DHT
#define DHTPIN 4          // Chân Data nối vào GPIO 4
#define DHTTYPE DHT11     // Khai báo loại cảm biến là DHT11

DHT dht(DHTPIN, DHTTYPE);
BluetoothSerial SerialBT;

void setup() {
    Serial.begin(115200);
    
    // Khởi tạo cảm biến DHT
    dht.begin();
    
    // Khởi tạo Bluetooth với tên thiết bị
    SerialBT.begin("ESP32_DHT11_Huy"); 
    Serial.println("Bluetooth da san sang! Hay ket noi voi 'ESP32_DHT11_Huy'");
}

void loop() {
    // Đọc dữ liệu (mất khoảng 250ms)
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Kiểm tra xem việc đọc có lỗi không
    if (isnan(h) || isnan(t)) {
        Serial.println("Loi doc cam bien DHT11!");
        return;
    }

    // Tạo chuỗi dữ liệu để gửi
    String dataToSend = "Nhiet do: " + String(t) + "C | Do am: " + String(h) + "%";

    // In ra Serial Monitor (để debug trên VS Code)
    Serial.println(dataToSend);

    // Gửi qua Bluetooth lên Máy tính/Điện thoại
    SerialBT.println(dataToSend);

    delay(2000); // Đợi 2 giây trước khi đọc lần tiếp theo
}