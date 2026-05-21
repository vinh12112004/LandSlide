/*
 * Mpu9250.cpp
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */


#include "MPU6500.h"

MPU6500::MPU6500(ISPI* p_spi, IGPIO* p_csPin) : spi(p_spi), csPin(p_csPin) {
    csPin->Write(true);
}

void MPU6500::WriteRegister(uint8_t regAddr, uint8_t data) {
    uint8_t tx[2] = { regAddr, data };
    uint8_t rx[2] = { 0, 0 };

    csPin->Write(false);
    spi->ReadWrite(tx, rx, 2);
    csPin->Write(true);
}

uint8_t MPU6500::ReadRegister(uint8_t regAddr) {
    uint8_t tx[2] = { (uint8_t)(regAddr | 0x80), 0x00 };
    uint8_t rx[2] = { 0, 0 };

    csPin->Write(false);
    for(volatile int i=0;i<50;i++);
    spi->ReadWrite(tx, rx, 2);
    csPin->Write(true);

    return rx[1];
}

void MPU6500::ReadRegisters(uint8_t regAddr, uint8_t* buffer, uint8_t len) {
    uint8_t txAddr = regAddr | 0x80;

    csPin->Write(false);

    uint8_t rxDummy;
    spi->ReadWrite(&txAddr, &rxDummy, 1);

    uint8_t txDummy = 0x00;
    for(uint8_t i = 0; i < len; i++) {
        spi->ReadWrite(&txDummy, &buffer[i], 1);
    }

    csPin->Write(true);
}

uint8_t MPU6500::Init() {
    WriteRegister(PWR_MGMT_1, 0x00);
    WriteRegister(0x6A, 0x10);
    for(volatile int i = 0; i < 10000; i++);
    uint8_t whoami = ReadRegister(WHO_AM_I);
    if (whoami == 0x71 || whoami == 0x73 || whoami == 0x70) {
        return whoami;
    }
    return whoami;
}

MPU6500::AccelData MPU6500::GetAccel() {
    uint8_t rawData[6];
    ReadRegisters(ACCEL_XOUT_H, rawData, 6);

    AccelData data;
    data.x = (int16_t)((rawData[0] << 8) | rawData[1]);
    data.y = (int16_t)((rawData[2] << 8) | rawData[3]);
    data.z = (int16_t)((rawData[4] << 8) | rawData[5]);

    return data;
}
MPU6500::GyroData MPU6500::GetGyro() {
    uint8_t raw[6];

    ReadRegisters(GYRO_XOUT_H, raw, 6);

    GyroData data;
    data.x = (int16_t)((raw[0] << 8) | raw[1]);
    data.y = (int16_t)((raw[2] << 8) | raw[3]);
    data.z = (int16_t)((raw[4] << 8) | raw[5]);

    return data;
}
void MPU6500::SetGyroRange(GyroRange range) {
    uint8_t config = ReadRegister(GYRO_CONFIG);
    config &= ~(0x03 << 3);
    config |= (range << 3);
    WriteRegister(GYRO_CONFIG, config);
}
void MPU6500::SetAccelRange(AccelRange range) {
    uint8_t config = ReadRegister(ACCEL_CONFIG);
    config &= ~(0x03 << 3);
    config |= (range << 3);
    WriteRegister(ACCEL_CONFIG, config);
}
void MPU6500::SetSampleRate(uint16_t rateHz) {
    if (rateHz > 1000) rateHz = 1000;
    if(rateHz == 0)
            return;
    uint8_t smplrt_div = (1000 / rateHz) - 1;
    WriteRegister(SMPLRT_DIV, smplrt_div);
}
void MPU6500::SetGyroDLPF(DLPFBandwidth bandwidth) {
	uint8_t gyroConfig = ReadRegister(GYRO_CONFIG);
	gyroConfig &= ~0x03;
	WriteRegister(GYRO_CONFIG, gyroConfig);

    uint8_t config = ReadRegister(CONFIG);
    config &= ~0x07;
    config |= bandwidth;
    WriteRegister(CONFIG, config);
}
void MPU6500::SetAccelDLPF(DLPFBandwidth bandwidth) {
    uint8_t config = ReadRegister(ACCEL_CONFIG_2);
    config &= ~0x0F;
    config |= bandwidth;
    WriteRegister(ACCEL_CONFIG_2, config);
}
void MPU6500::EnableDataReadyInterrupt()
{
    WriteRegister(INT_PIN_CFG, 0x00);
    WriteRegister(INT_ENABLE, 0x01);
}
