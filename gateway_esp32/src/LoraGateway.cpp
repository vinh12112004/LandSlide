#include "LoraGateway.h"
#include "config.h"

HardwareSerial LoRaSerial(2);

static void waitAux()
{
    while (digitalRead(LORA_AUX) == LOW) {
        delay(1);
    }
}

static void setMode(LoRaMode mode)
{
    switch(mode)
    {
        case NORMAL:
            digitalWrite(LORA_M0, LOW);
            digitalWrite(LORA_M1, LOW);
            break;

        case WAKEUP:
            digitalWrite(LORA_M0, HIGH);
            digitalWrite(LORA_M1, LOW);
            break;

        case POWERSAVE:
            digitalWrite(LORA_M0, LOW);
            digitalWrite(LORA_M1, HIGH);
            break;

        case CONFIG:
            digitalWrite(LORA_M0, HIGH);
            digitalWrite(LORA_M1, HIGH);
            break;
    }

    delay(50);
    waitAux();
}

void Lora_Init()
{
    pinMode(LORA_M0, OUTPUT);
    pinMode(LORA_M1, OUTPUT);
    pinMode(LORA_AUX, INPUT);
    LoRaSerial.setRxBufferSize(512);
    LoRaSerial.begin(
        9600,
        SERIAL_8N1,
        LORA_RX,
        LORA_TX
    );

    setMode(NORMAL);
    while(LoRaSerial.available() > 0) {
        LoRaSerial.read();
    }
    Serial.println("[LORA] Ready");
    Serial.printf("[LORA] Packet size: %d bytes\r\n", sizeof(PacketData_t));
}
// Hàm tính XOR Checksum
uint8_t CalculateChecksum(const uint8_t* data, uint16_t length)
{
    uint8_t checksum = 0;
    // Tính từ đầu gói tin cho đến sát byte checksum cuối cùng
    for (uint16_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}
bool Lora_ReadPacket(PacketData_t *packet)
{
    static uint8_t rxBuffer[128];
    static uint16_t rxIndex = 0;
    static unsigned long lastRxTime = 0;

    if (rxIndex > 0 && millis() - lastRxTime > 1000) {
        rxIndex = 0;
    }

    while (LoRaSerial.available() > 0)
    {
        uint8_t b = LoRaSerial.read();
        lastRxTime = millis();

        // tìm header 55 AA
        if (rxIndex == 0) {
            if (b != 0x55) continue;
            rxBuffer[rxIndex++] = b;
            continue;
        }

        if (rxIndex == 1) {
            if (b == 0xAA) {
                rxBuffer[rxIndex++] = b;
            } else {
                rxIndex = 0;
            }
            continue;
        }

        rxBuffer[rxIndex++] = b;

        if (rxIndex == sizeof(PacketData_t))
        {
            uint8_t calculatedChecksum =
                CalculateChecksum(rxBuffer, sizeof(PacketData_t) - 1);

            uint8_t receivedChecksum =
                rxBuffer[sizeof(PacketData_t) - 1];

            if (calculatedChecksum == receivedChecksum)
            {
                memcpy(packet, rxBuffer, sizeof(PacketData_t));
                packet->deviceId[15] = '\0';
                rxIndex = 0;
                return true;
            }

            Serial.printf("[LORA] Checksum Failed! Calc: %02X, Recv: %02X\r\n",
                          calculatedChecksum, receivedChecksum);
            
            // --- IN RAW DATA ĐỂ DEBUG LỆCH STRUCT ---
            Serial.print("[LORA] Raw Data: ");
            for (uint16_t i = 0; i < sizeof(PacketData_t); i++) {
                // Thêm số 0 ở trước nếu số hex chỉ có 1 chữ số (VD: 0F thay vì F)
                if (rxBuffer[i] < 0x10) Serial.print("0");
                Serial.print(rxBuffer[i], HEX);
                Serial.print(" ");
            }
            Serial.println(); // Xuống dòng
            // -----------------------------------------

            rxIndex = 0;   // reset hẳn, không shift từng byte nữa
        }

        if (rxIndex >= sizeof(rxBuffer)) {
            rxIndex = 0;
        }
    }

    return false;
}
void Lora_PrintPacket(const PacketData_t &packet)
{
    Serial.println("========== LORA PACKET ==========");
    Serial.printf("Device : %s\r\n", packet.deviceId);
    Serial.printf("Mask   : 0x%02X\r\n", packet.sensorMask);

    if (packet.sensorMask & HAS_TILT)
        Serial.printf("Tilt   : %.2f\r\n", packet.tilt);

    if (packet.sensorMask & HAS_VIB)
        Serial.printf("Vib    : %.3f\r\n", packet.vib);

    if (packet.sensorMask & HAS_SOIL)
        Serial.printf("Soil   : %.2f\r\n", packet.soil_m);

    if (packet.sensorMask & HAS_GPS)
    {
        Serial.println("----- GPS -----");
        Serial.printf("Fix    : %u\r\n", packet.fixType);
        Serial.printf("SV     : %u\r\n", packet.numSV);
        Serial.printf("Lat    : %.7f\r\n", packet.lat / 10000000.0);
        Serial.printf("Lon    : %.7f\r\n", packet.lon / 10000000.0);
        Serial.printf("hMSL   : %.2f m\r\n", packet.hMSL);
        Serial.printf("hAcc   : %.2f m\r\n", packet.hAcc);
        Serial.printf("vAcc   : %.2f m\r\n", packet.vAcc);
        Serial.printf("pDOP   : %.2f\r\n", packet.pDOP);
    }

    Serial.println();
}
