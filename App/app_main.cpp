/*
 * app_main.cpp
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */
#include "app_main.h"
#include <math.h>
#include <string.h>
#include "iostream"
#include "cmsis_os.h"
#include "SoilMoistureSensor.h"
#include "AS32.h"
#include "MPU6500.h"
#include "STM32_GPIO.h"
#include "STM32_SPI.h"
#include "STM32_UART.h"
#include "STM32_ADC.h"
#include "ZEDF9K.h"
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern SPI_HandleTypeDef hspi2;
extern ADC_HandleTypeDef hadc2;
static const char DEVICE_ID[] = "STM32_HN_01";
//mutex
IMUData_t imuData;
SoilData_t soilData;

osMutexId_t imuMutex;
osMutexId_t soilMutex;
osMutexId_t gpsMutex;
// Debug
uint8_t isConnected;
bool isSenđing;
const char* msg = "pablo11\r\n";
ZEDF9K::NavPvtData nav;
volatile uint8_t gpsNavPvtCount;
volatile uint32_t gpsRxOkCount = 0;

// AS32
STM32_GPIO pin_m0(GPIOB, GPIO_PIN_0);
STM32_GPIO pin_m1(GPIOB, GPIO_PIN_1);
STM32_GPIO pin_aux(GPIOB, GPIO_PIN_10);
STM32_UART as32_uart(&huart1);
AS32 lora(&pin_m0, &pin_m1, &pin_aux, &as32_uart);

// MPU9250
STM32_SPI mpu_spi(&hspi2);
STM32_GPIO mpu_cs(GPIOA, GPIO_PIN_4);
MPU6500 imu(&mpu_spi, &mpu_cs);
float accOffsetX = 0;
float accOffsetY = 0;
float accOffsetZ = 0;
bool imuCalibrated = false;
uint16_t calibCount = 0;

int64_t calibSumX = 0;
int64_t calibSumY = 0;
int64_t calibSumZ = 0;

#define IMU_CALIB_SAMPLES 1000
// SOIL
STM32_ADC soil_adc(&hadc2);
SoilMoistureSensor soil(&soil_adc, 3000, 1200);
uint16_t raw ;

// ZED-F9K
STM32_UART gps_uart(&huart2);
ZEDF9K gps(&gps_uart);

void App_Init(void)
{
	imuMutex = osMutexNew(NULL);
	soilMutex = osMutexNew(NULL);
	gpsMutex = osMutexNew(NULL);
	lora.Init();
	isConnected = imu.Init();
	if(isConnected){
		imu.SetAccelRange(ACCEL_2G);
		imu.SetGyroRange(GYRO_250DPS);
		imu.SetGyroDLPF(DLPF_41HZ);
		imu.SetAccelDLPF(DLPF_41HZ);
		imu.SetSampleRate(10);
		imu.EnableDataReadyInterrupt();

	}
	gps.Init();
}


void IMU_Process(void)
{
    if(!isConnected) return;

    auto acc = imu.GetAccel();
    auto gyro = imu.GetGyro();
    if(!imuCalibrated)
    {
        calibSumX += acc.x;
        calibSumY += acc.y;
        calibSumZ += acc.z;
        calibCount++;

        if(calibCount >= IMU_CALIB_SAMPLES)
        {
            accOffsetX = calibSumX / (float)IMU_CALIB_SAMPLES;
            accOffsetY = calibSumY / (float)IMU_CALIB_SAMPLES;
            accOffsetZ = (calibSumZ / (float)IMU_CALIB_SAMPLES) - 16384.0f;

            imuCalibrated = true;
        }

        return;
    }

    float ax = (acc.x - accOffsetX) / 16384.0f;
    float ay = (acc.y - accOffsetY) / 16384.0f;
    float az = (acc.z - accOffsetZ) / 16384.0f;

    float gx = gyro.x / 131.0f;
	float gy = gyro.y / 131.0f;
	float gz = gyro.z / 131.0f;

	float gyroMag = sqrtf(gx*gx + gy*gy + gz*gz);

	static float ax_g = 0.0f;
	static float ay_g = 0.0f;
	static float az_g = 1.0f;

	float alpha = 0.02f;

	if(gyroMag < 5.0f)
	{
		ax_g += alpha * (ax - ax_g);
		ay_g += alpha * (ay - ay_g);
		az_g += alpha * (az - az_g);
	}

	float ax_v = ax - ax_g;
	float ay_v = ay - ay_g;
	float az_v = az - az_g;

	float vib_inst = sqrtf(
		ax_v * ax_v +
		ay_v * ay_v +
		az_v * az_v
	) * 9.81f;

	if(gyroMag > 20.0f)
	{
		vib_inst *= 0.3f;
	}

	static float vib_f = 0.0f;
	vib_f = vib_f * 0.9f + vib_inst * 0.1f;

	IMUData_t temp;

	temp.tilt = atan2f(
		sqrtf(ax * ax + ay * ay),
		az
	) * 180.0f / 3.14159265f;

	temp.vib = vib_f;



    osMutexAcquire(imuMutex, osWaitForever);
    imuData = temp;
    osMutexRelease(imuMutex);
}
void LoRa_Process(void)
{
    PacketData_t packet;
    memset(&packet, 0, sizeof(packet));

    packet.header = PACKET_HEADER;

    strncpy(packet.deviceId, DEVICE_ID, sizeof(packet.deviceId) - 1);

    osMutexAcquire(imuMutex, osWaitForever);
    packet.tilt = imuData.tilt;
    packet.vib  = imuData.vib;
    packet.sensorMask |= HAS_TILT | HAS_VIB;
    osMutexRelease(imuMutex);

    osMutexAcquire(soilMutex, osWaitForever);
    packet.soil_m = soilData.soil_m;
    packet.sensorMask |= HAS_SOIL;
    osMutexRelease(soilMutex);

    osMutexAcquire(gpsMutex, osWaitForever);
    packet.lat = nav.lat;
    packet.lon = nav.lon;
    packet.hMSL = nav.hMSL;
    packet.hAcc = nav.hAcc;
    packet.vAcc = nav.vAcc;
    packet.pDOP = nav.pDOP;
    packet.fixType = nav.fixType;
    packet.numSV = nav.numSV;

//    if (nav.fixType >= 3)
//    {
//        packet.sensorMask |= HAS_GPS;
//    }
    packet.sensorMask |= HAS_GPS;

    osMutexRelease(gpsMutex);

    isSenđing = true;
    lora.SendData((uint8_t*)&packet, sizeof(packet));
    isSenđing = false;
}

void SOIL_Process(void)
{
	raw = soil.ReadRawAverage(10);

    SoilData_t temp;

    temp.soil_m = soil.ReadMoisturePercent();

    osMutexAcquire(soilMutex, osWaitForever);
    soilData = temp;
    osMutexRelease(soilMutex);
}

void GPS_Process(void)
{
	if (gps.HasNewNavPvt())
	    {
	        ZEDF9K::NavPvtData temp = gps.GetNavPvt();

	        osMutexAcquire(gpsMutex, osWaitForever);
	        nav = temp;
	        osMutexRelease(gpsMutex);

	        gpsNavPvtCount++;
	    }
}
extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        gpsRxOkCount++;
        gps.OnRxByte();
    }
}
