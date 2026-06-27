#ifndef LORA_PACKET_H
#define LORA_PACKET_H

#include <Arduino.h>

#define PACKET_HEADER 0xAA55

#define HAS_TILT  0x01
#define HAS_VIB   0x02
#define HAS_SOIL  0x04
#define HAS_GPS   0x08

#pragma pack(push, 1)
typedef struct __attribute__((packed))
{
    uint16_t header;       // 2 bytes
    char deviceId[16];     // 16 bytes
    uint16_t sensorMask;   // 2 bytes
    
    float tilt;            // 4 bytes
    float vib;             // 4 bytes
    float soil_m;          // 4 bytes
    
    int32_t lat;           // 4 bytes (ĐỔI TỪ double)
    int32_t lon;           // 4 bytes (ĐỔI TỪ double)
    float hMSL;            // 4 bytes
    float hAcc;            // 4 bytes
    float vAcc;            // 4 bytes
    float pDOP;            // 4 bytes
    
    uint8_t fixType;       // 1 byte
    uint8_t numSV;         // 1 byte
    uint8_t checksum;      // 1 byte
} PacketData_t;            // TỔNG CỘNG ĐÚNG 59 BYTES
#pragma pack(pop)
static_assert(sizeof(PacketData_t) == 59, "PacketData_t must be 59 bytes");
#endif