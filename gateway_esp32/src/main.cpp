#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include "LoraGateway.h"
#include "MqttService.h"

// Khai báo Queue Handle để chứa các gói tin LoRa
QueueHandle_t packetQueue;

// Tọa độ gốc giả lập nhân với 10^7 (21.028511, 105.804817)
int32_t simLat = 210285110;
int32_t simLon = 1058048170;

// ==========================================================
// TASK GIẢ LẬP LORA BẰNG SERIAL MONITOR
// ==========================================================
void vTaskSimulateLoRa(void *pvParameters)
{
    PacketData_t packet;
    
    // Gán các giá trị Header và DeviceID cơ bản
    packet.header = PACKET_HEADER;
    strncpy(packet.deviceId, "TEST_DEVICE_01", sizeof(packet.deviceId));
    packet.sensorMask = HAS_TILT | HAS_GPS | HAS_SOIL;
    
    // Gán giá trị mặc định cho các cảm biến khác (khớp với tên biến trong struct)
    packet.tilt = 0.0f;
    packet.vib = 0.0f;
    packet.soil_m = 50.0f;
    packet.hMSL = 15.5f;
    packet.fixType = 3; // 3D Fix

    for (;;)
    {
        if (Serial.available())
        {
            char cmd = Serial.read();
            bool sendNow = false;

            // Chú ý: 1 đơn vị ở int32_t (10^-7 độ) tương đương khoảng 1.1 cm thực tế
            if (cmd == '1') {
                Serial.println("\n>> [TEST] Kịch bản 1: Đứng yên (Nhiễu ~2cm)");
                packet.lat = simLat + 2; 
                packet.lon = simLon;
                sendNow = true;
            }
            else if (cmd == '2') {
                Serial.println("\n>> [TEST] Kịch bản 2: Sạt lở CHẬM (Dịch ~22cm -> Vượt ngưỡng Level 1)");
                packet.lat = simLat + 20; 
                packet.lon = simLon;
                sendNow = true;
            }
            else if (cmd == '3') {
                Serial.println("\n>> [TEST] Kịch bản 3: Sạt lở NHANH (Dịch ~1.1m -> Vượt ngưỡng Level 2)");
                packet.lat = simLat + 100; 
                packet.lon = simLon;
                sendNow = true;
            }

            if (sendNow) {
                // Tính checksum đơn giản (tùy chọn)
                packet.checksum = 0; 
                
                if (xQueueSend(packetQueue, &packet, 10 / portTICK_PERIOD_MS) != pdPASS) {
                    Serial.println("[LORA] Queue đầy!");
                } else {
                    Serial.println("   Đã nạp gói tin vào Queue!");
                }
            }
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS); 
    }
}
// Task xử lý LoRa
void vTaskLoRa(void *pvParameters)
{
    PacketData_t packet;
    
    for (;;)
    {
        // Đọc gói tin liên tục
        if (Lora_ReadPacket(&packet))
        {
            Lora_PrintPacket(packet);
            
            // Đẩy gói tin vào Queue để gửi sang Task MQTT
            // Thời gian chờ tối đa (Tick) nếu Queue đang đầy: 10 Tick
            if (xQueueSend(packetQueue, &packet, 10 / portTICK_PERIOD_MS) != pdPASS) {
                Serial.println("[LORA] Queue đầy, rớt gói tin!");
            }
        }
        
        // Nhường CPU 1 chút (tránh hiện tượng Watchdog Timer panic)
        vTaskDelay(5 / portTICK_PERIOD_MS); 
    }
}

// Task xử lý MQTT & Mạng
void vTaskMQTT(void *pvParameters)
{
    PacketData_t packet;
    
    for (;;)
    {
        // Liên tục duy trì kết nối MQTT
        MQTT_Loop();

        // Chờ nhận dữ liệu từ Queue (chờ tối đa 50ms)
        if (xQueueReceive(packetQueue, &packet, 50 / portTICK_PERIOD_MS) == pdPASS)
        {
            // Đã lấy được gói tin từ Queue, tiến hành publish
            MQTT_PublishSensor(packet);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("===== ESP32 LANDSLIDE GATEWAY =====");

    // Khởi tạo các ngoại vi
    Lora_Init();
    Wifi_Connect();
    Time_Sync();
    MQTT_Init();

    Serial.println("[SYSTEM] Init Done. Starting FreeRTOS Tasks...");

    // Tạo Queue chứa tối đa 10 gói tin (mỗi gói có kích thước sizeof(PacketData_t) = 59 bytes)
    packetQueue = xQueueCreate(10, sizeof(PacketData_t));
    if (packetQueue == NULL) {
        Serial.println("[SYSTEM] Lỗi tạo Queue!");
        while (1); // Halt nếu không tạo được RAM cho Queue
    }

    // Khởi tạo Task LoRa ghim vào Core 1 (App Core)
    // xTaskCreatePinnedToCore(
    //     vTaskLoRa,       // Hàm thực thi của Task
    //     "Task_LoRa",     // Tên Task (để debug)
    //     4096,            // Kích thước Stack (Bytes)
    //     NULL,            // Tham số truyền vào
    //     2,               // Độ ưu tiên (Priority - số lớn là ưu tiên cao)
    //     NULL,            // Task Handle
    //     1                // Ghim vào Core 1
    // );
    xTaskCreatePinnedToCore(vTaskSimulateLoRa, "Task_SimLoRa", 4096, NULL, 2, NULL, 1);
    // Khởi tạo Task MQTT ghim vào Core 0 (Pro Core - chuyên xử lý Network)
    xTaskCreatePinnedToCore(
        vTaskMQTT,
        "Task_MQTT",
        8192,            // Xử lý chuỗi JSON và WiFi nên cần Stack lớn hơn một chút
        NULL,
        1,               // Độ ưu tiên thấp hơn LoRa để tránh lỡ mất tín hiệu
        NULL,
        0                // Ghim vào Core 0
    );

    Serial.println("[SYSTEM] Gateway ready");
}

void loop()
{
    // Hàm loop() trong Arduino bản chất cũng là một Task của FreeRTOS.
    // Vì ta đã đưa logic vào vTaskLoRa và vTaskMQTT, hàm loop() không cần làm gì cả.
    // Ta xóa task này đi để giải phóng tài nguyên.
    vTaskDelete(NULL);
}