/*
 * app_main.cpp
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */
#include "app_main.h"

#include <string.h>
#include "iostream"
#include "cmsis_os.h"
#include "queue.h"

#include "AS32.h"
#include "MPU6500.h"
#include "STM32_GPIO.h"
#include "STM32_SPI.h"
#include "STM32_UART.h"
extern UART_HandleTypeDef huart1;
extern SPI_HandleTypeDef hspi2;

// Debug
bool isConnected;
float ax;
float ay;
float az;
float gx;
float gy;
float gz;
uint8_t whoami;
IMUData_t data;
const char* msg = "pablo11\r\n";
//queue
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

// App Init
void App_Init(void)
{
	lora.Init();
	isConnected = imu.Init();
	whoami = imu.ReadRegister(WHO_AM_I);
//	whoami = 1;

	if(isConnected){
		imu.SetAccelRange(ACCEL_2G);
		imu.SetGyroRange(GYRO_250DPS);
		imu.SetGyroDLPF(DLPF_41HZ);
		imu.SetAccelDLPF(DLPF_41HZ);
		imu.SetSampleRate(10);
		imu.EnableDataReadyInterrupt();
	}
}

// App Loop
void App_Loop(void) {
	char buf[50];
	auto acc = imu.GetAccel();
	auto gyro = imu.GetGyro();
	ax = acc.x / 16384.0f;
	ay = acc.y / 16384.0f;
	az = acc.z / 16384.0f;
	gx = gyro.x / 16384.0f;
	gy = gyro.y / 16384.0f;
	gz = gyro.z / 16384.0f;
	snprintf(buf, sizeof(buf),
	    "A:%.2d %.2d %.2d G:%.2d %.2d %.2d",
	    ax, ay, az,
	    gx, gy, gz
	);
	lora.SendData((uint8_t*)buf, strlen(buf));
    HAL_Delay(500);
}
void IMU_Process()
{
    auto acc = imu.GetAccel();
    auto gyro = imu.GetGyro();

    data.ax = acc.x / 16384.0f;
    data.ay = acc.y / 16384.0f;
    data.az = acc.z / 16384.0f;

    data.gx = gyro.x / 131.0f;
    data.gy = gyro.y / 131.0f;
    data.gz = gyro.z / 131.0f;

//    osMessageQueuePut(imuQueueHandle, &data, 0, 0);
}
void LoRa_Process()
{
	IMUData_t local = data;
	lora.SendData((uint8_t*)&local, sizeof(local));
}
