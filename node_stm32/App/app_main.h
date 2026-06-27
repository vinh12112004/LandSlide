/*
 * app_main.h
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */

#ifndef APP_MAIN_H_
#define APP_MAIN_H_
#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    float tilt;
    float vib;
} IMUData_t;

typedef struct
{
    float soil_m;
} SoilData_t;

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

#define PACKET_HEADER 0xAA55
#define HAS_TILT  0x01
#define HAS_VIB   0x02
#define HAS_SOIL  0x04
#define HAS_GPS   0x08

void App_Init(void);
void App_Loop(void);
void IMU_Process(void);
void SOIL_Process();
void LoRa_Process();
void GPS_Process();
void IMU_Trigger_DMA_Read(void);
void SOIL_Trigger_DMA(void);
void App_GPS_Init(void);
#ifdef __cplusplus
}
#endif


#endif /* APP_MAIN_H_ */
