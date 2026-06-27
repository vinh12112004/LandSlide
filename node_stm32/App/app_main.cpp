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
extern SPI_HandleTypeDef hspi1;
extern ADC_HandleTypeDef hadc1;
//task
extern osThreadId_t IMUTaskHandle;
extern osThreadId_t SoilTaskHandle;
extern osThreadId_t GNSSTaskHandle;
//ID
static const char DEVICE_ID[] = "STM32_HN_01";
//mutex
IMUData_t imuData;
SoilData_t soilData;

osMutexId_t imuMutex;
osMutexId_t soilMutex;
osMutexId_t gpsMutex;
// Debug
uint8_t isConnected;
bool isSending;
const char* msg = "pablo11\r\n";
ZEDF9K::NavPvtData nav;
volatile uint8_t gpsNavPvtCount;
volatile uint32_t gpsRxOkCount = 0;

// AS32
STM32_GPIO pin_m0(GPIOA, GPIO_PIN_12);
STM32_GPIO pin_m1(GPIOA, GPIO_PIN_11);
STM32_GPIO pin_aux(GPIOA, GPIO_PIN_8);
STM32_UART as32_uart(&huart1);
AS32 lora(&pin_m0, &pin_m1, &pin_aux, &as32_uart);

// MPU6500
MPU6500::AccelData acc;
MPU6500::GyroData gyro;
STM32_SPI mpu_spi(&hspi1);
STM32_GPIO mpu_cs(GPIOA, GPIO_PIN_4);
MPU6500 imu(&mpu_spi, &mpu_cs);
//float accOffsetX = 0;
//float accOffsetY = 0;
//float accOffsetZ = 0;
//bool imuCalibrated = false;
//uint16_t calibCount = 0;
//
//int64_t calibSumX = 0;
//int64_t calibSumY = 0;
//int64_t calibSumZ = 0;

#define IMU_CALIB_SAMPLES 1000

// SOIL
STM32_ADC soil_adc(&hadc1);
SoilMoistureSensor soil(&soil_adc, 543, 1240);
uint16_t raw ;

// ZED-F9K
STM32_UART gps_uart(&huart2);
ZEDF9K gps(&gps_uart);
volatile uint16_t gps_dma_current_pos = 0;
uint8_t debug_gps_raw[20];
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
//	HAL_Delay(2000);
	gps.Init();
//	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, gps.rxBuffer, GPS_RX_BUFFER_SIZE);

}
void App_GPS_Init(void)
{
    // Bắt đầu gửi lệnh cấu hình UBX cho ZED-F9K
    gps.Init();
    // Bắt đầu cho DMA hứng dữ liệu
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, gps.rxBuffer, GPS_RX_BUFFER_SIZE);
}
uint8_t CalculateChecksum(const uint8_t* data, uint16_t length)
{
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void IMU_Process(void)
{
    if(!isConnected) return;

    MPU6500::ProcessedData processed;

    if (imu.ProcessSensorMath(&processed))
    {
        osMutexAcquire(imuMutex, osWaitForever);
        imuData.tilt = processed.tilt;
        imuData.vib  = processed.vib;
        osMutexRelease(imuMutex);
    }
}

//void GPS_Process(void)
//{
//	if (gps.HasNewNavPvt())
//	    {
//	        ZEDF9K::NavPvtData temp = gps.GetNavPvt();
//
//	        osMutexAcquire(gpsMutex, osWaitForever);
//	        nav = temp;
//	        osMutexRelease(gpsMutex);
//
//	        gpsNavPvtCount++;
//	    }
//}
//extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//    if (huart->Instance == USART2)
//    {
//        gpsRxOkCount++;
//        gps.OnRxByte();
//    }
//}
void GPS_Process(void)
{
    // Yêu cầu phân tích những byte DMA vừa nhặt được
	if (gps_dma_current_pos > 0 && gps_dma_current_pos <= 512) {
	        for(int i = 0; i < 20; i++) {
	            debug_gps_raw[i] = gps.rxBuffer[i];
	        }
	    }
    gps.ProcessCircularBuffer(gps_dma_current_pos);

    if (gps.HasNewNavPvt())
    {
        ZEDF9K::NavPvtData temp = gps.GetNavPvt();

        osMutexAcquire(gpsMutex, osWaitForever);
        nav = temp;
        osMutexRelease(gpsMutex);

        gpsNavPvtCount++;
    }
}
extern "C" void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART2)
    {
        gps_dma_current_pos = Size;
        osThreadFlagsSet(GNSSTaskHandle, 0x01);
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, gps.rxBuffer, GPS_RX_BUFFER_SIZE);
		__HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
}
extern "C" void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        imu.EndDMARead();
        osThreadFlagsSet(IMUTaskHandle, 0x02);
    }
}

extern "C" void IMU_Trigger_DMA_Read(void)
{
    if (isConnected)
    {
        imu.TriggerDMARead();
    }
}

extern "C" void SOIL_Trigger_DMA(void)
{
    soil.TriggerDMARead();
}

extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc->Instance == ADC1)
    {
        HAL_ADC_Stop_DMA(hadc);
        osThreadFlagsSet(SoilTaskHandle, 0x01);
    }
}

void SOIL_Process(void)
{
    soil.ProcessDMAData();
    raw = soil.GetRawAverage();
    SoilData_t temp;
    temp.soil_m = soil.GetMoisturePercent();

    osMutexAcquire(soilMutex, osWaitForever);
    soilData = temp;
    osMutexRelease(soilMutex);
}

extern "C" void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        lora.OnTxComplete();
        isSending = false;

    }
}
PacketData_t packet;

void LoRa_Process(void)
{
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
    uint16_t packetLen = sizeof(PacketData_t);
    packet.checksum = 0;
    packet.checksum = CalculateChecksum((uint8_t*)&packet, packetLen - 1);
    isSending = true;
    if (!lora.SendDataIT((uint8_t*)&packet, sizeof(packet)))
    {
        isSending = false;
    }
}


