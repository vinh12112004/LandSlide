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

typedef struct __attribute__((packed))
{
    uint16_t header;

    char deviceId[16];
    uint16_t sensorMask;

    float tilt;
    float vib;

    float soil_m;

    double lat;
	double lon;
	float hMSL;
	float hAcc;
	float vAcc;
	float pDOP;

	uint8_t fixType;
	uint8_t numSV;
} PacketData_t;
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
#ifdef __cplusplus
}
#endif


#endif /* APP_MAIN_H_ */
