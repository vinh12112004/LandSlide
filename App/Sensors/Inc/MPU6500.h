/*
 * Mpu9250.h
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */

#ifndef SENSORS_INC_MPU6500_H_
#define SENSORS_INC_MPU6500_H_

#include "../../Protocols/Inc/Interface/IGPIO.h"
#include "../../Protocols/Inc/Interface/ISPI.h"

/* =========================
 * Self Test Registers
 * ========================= */
#define SELF_TEST_X_GYRO    0x00    // Gyroscope X-axis self test
#define SELF_TEST_Y_GYRO    0x01    // Gyroscope Y-axis self test
#define SELF_TEST_Z_GYRO    0x02    // Gyroscope Z-axis self test

#define SELF_TEST_X_ACCEL   0x0D    // Accelerometer X-axis self test
#define SELF_TEST_Y_ACCEL   0x0E    // Accelerometer Y-axis self test
#define SELF_TEST_Z_ACCEL   0x0F    // Accelerometer Z-axis self test

/* =========================
 * Gyroscope Offset Registers
 * ========================= */
#define XG_OFFSET_H         0x13    // Gyro X offset high byte
#define XG_OFFSET_L         0x14    // Gyro X offset low byte

#define YG_OFFSET_H         0x15    // Gyro Y offset high byte
#define YG_OFFSET_L         0x16    // Gyro Y offset low byte

#define ZG_OFFSET_H         0x17    // Gyro Z offset high byte
#define ZG_OFFSET_L         0x18    // Gyro Z offset low byte

/* =========================
 * Configuration Registers
 * ========================= */
#define SMPLRT_DIV          0x19    // Sample rate divider
#define CONFIG              0x1A    // Digital low pass filter config
#define GYRO_CONFIG         0x1B    // Gyroscope configuration
#define ACCEL_CONFIG        0x1C    // Accelerometer configuration
#define ACCEL_CONFIG_2      0x1D    // Accelerometer filter configuration
#define LP_ACCEL_ODR        0x1E    // Low power accel output data rate
#define WOM_THR             0x1F    // Wake-on-motion threshold

/* =========================
 * FIFO Registers
 * ========================= */
#define FIFO_EN             0x23    // FIFO enable register

/* =========================
 * I2C Master Registers
 * ========================= */
#define I2C_MST_CTRL        0x24    // I2C master control

#define I2C_SLV0_ADDR       0x25    // I2C slave 0 address
#define I2C_SLV0_REG        0x26    // I2C slave 0 register
#define I2C_SLV0_CTRL       0x27    // I2C slave 0 control

#define I2C_SLV1_ADDR       0x28    // I2C slave 1 address
#define I2C_SLV1_REG        0x29    // I2C slave 1 register
#define I2C_SLV1_CTRL       0x2A    // I2C slave 1 control

#define I2C_SLV2_ADDR       0x2B    // I2C slave 2 address
#define I2C_SLV2_REG        0x2C    // I2C slave 2 register
#define I2C_SLV2_CTRL       0x2D    // I2C slave 2 control

#define I2C_SLV3_ADDR       0x2E    // I2C slave 3 address
#define I2C_SLV3_REG        0x2F    // I2C slave 3 register
#define I2C_SLV3_CTRL       0x30    // I2C slave 3 control

#define I2C_SLV4_ADDR       0x31    // I2C slave 4 address
#define I2C_SLV4_REG        0x32    // I2C slave 4 register
#define I2C_SLV4_DO         0x33    // I2C slave 4 data out
#define I2C_SLV4_CTRL       0x34    // I2C slave 4 control
#define I2C_SLV4_DI         0x35    // I2C slave 4 data in

#define I2C_MST_STATUS      0x36    // I2C master status

/* =========================
 * Interrupt Registers
 * ========================= */
#define INT_PIN_CFG         0x37    // Interrupt pin configuration
#define INT_ENABLE          0x38    // Interrupt enable
#define INT_STATUS          0x3A    // Interrupt status

/* =========================
 * Accelerometer Output Registers
 * ========================= */
#define ACCEL_XOUT_H        0x3B    // Accel X high byte
#define ACCEL_XOUT_L        0x3C    // Accel X low byte

#define ACCEL_YOUT_H        0x3D    // Accel Y high byte
#define ACCEL_YOUT_L        0x3E    // Accel Y low byte

#define ACCEL_ZOUT_H        0x3F    // Accel Z high byte
#define ACCEL_ZOUT_L        0x40    // Accel Z low byte

/* =========================
 * Temperature Registers
 * ========================= */
#define TEMP_OUT_H          0x41    // Temperature high byte
#define TEMP_OUT_L          0x42    // Temperature low byte

/* =========================
 * Gyroscope Output Registers
 * ========================= */
#define GYRO_XOUT_H         0x43    // Gyro X high byte
#define GYRO_XOUT_L         0x44    // Gyro X low byte

#define GYRO_YOUT_H         0x45    // Gyro Y high byte
#define GYRO_YOUT_L         0x46    // Gyro Y low byte

#define GYRO_ZOUT_H         0x47    // Gyro Z high byte
#define GYRO_ZOUT_L         0x48    // Gyro Z low byte

/* =========================
 * External Sensor Data Registers
 * ========================= */
#define EXT_SENS_DATA_00    0x49
#define EXT_SENS_DATA_01    0x4A
#define EXT_SENS_DATA_02    0x4B
#define EXT_SENS_DATA_03    0x4C
#define EXT_SENS_DATA_04    0x4D
#define EXT_SENS_DATA_05    0x4E
#define EXT_SENS_DATA_06    0x4F
#define EXT_SENS_DATA_07    0x50
#define EXT_SENS_DATA_08    0x51
#define EXT_SENS_DATA_09    0x52
#define EXT_SENS_DATA_10    0x53
#define EXT_SENS_DATA_11    0x54
#define EXT_SENS_DATA_12    0x55
#define EXT_SENS_DATA_13    0x56
#define EXT_SENS_DATA_14    0x57
#define EXT_SENS_DATA_15    0x58
#define EXT_SENS_DATA_16    0x59
#define EXT_SENS_DATA_17    0x5A
#define EXT_SENS_DATA_18    0x5B
#define EXT_SENS_DATA_19    0x5C
#define EXT_SENS_DATA_20    0x5D
#define EXT_SENS_DATA_21    0x5E
#define EXT_SENS_DATA_22    0x5F
#define EXT_SENS_DATA_23    0x60

/* =========================
 * I2C Slave Data Out Registers
 * ========================= */
#define I2C_SLV0_DO         0x63    // I2C slave 0 data out
#define I2C_SLV1_DO         0x64    // I2C slave 1 data out
#define I2C_SLV2_DO         0x65    // I2C slave 2 data out
#define I2C_SLV3_DO         0x66    // I2C slave 3 data out

#define I2C_MST_DELAY_CTRL  0x67    // I2C master delay control

/* =========================
 * Signal Path / User Control
 * ========================= */
#define SIGNAL_PATH_RESET   0x68    // Signal path reset
#define ACCEL_INTEL_CTRL    0x69    // Motion detection control
#define USER_CTRL           0x6A    // User control register

/* =========================
 * Power Management Registers
 * ========================= */
#define PWR_MGMT_1          0x6B    // Power management 1
#define PWR_MGMT_2          0x6C    // Power management 2

/* =========================
 * FIFO Count Registers
 * ========================= */
#define FIFO_COUNT_H        0x72    // FIFO count high byte
#define FIFO_COUNT_L        0x73    // FIFO count low byte
#define FIFO_R_W            0x74    // FIFO read/write

/* =========================
 * Device ID Register
 * ========================= */
#define WHO_AM_I            0x75    // Device ID register

/* =========================
 * Accelerometer Offset Registers
 * ========================= */
#define XA_OFFSET_H         0x77    // Accel X offset high byte
#define XA_OFFSET_L         0x78    // Accel X offset low byte

#define YA_OFFSET_H         0x7A    // Accel Y offset high byte
#define YA_OFFSET_L         0x7B    // Accel Y offset low byte

#define ZA_OFFSET_H         0x7D    // Accel Z offset high byte
#define ZA_OFFSET_L         0x7E    // Accel Z offset low byte

enum GyroRange
{
    GYRO_250DPS = 0,
    GYRO_500DPS,
    GYRO_1000DPS,
    GYRO_2000DPS
};
enum AccelRange
{
    ACCEL_2G = 0,
    ACCEL_4G,
    ACCEL_8G,
    ACCEL_16G
};
enum DLPFBandwidth
{
    DLPF_250HZ = 0,
    DLPF_184HZ = 1,
    DLPF_92HZ  = 2,
    DLPF_41HZ  = 3,
    DLPF_20HZ  = 4,
    DLPF_10HZ  = 5,
    DLPF_5HZ   = 6,
    DLPF_3600HZ = 7
};

class MPU6500 {
private:
    ISPI* spi;
    IGPIO* csPin;

    void WriteRegister(uint8_t regAddr, uint8_t data);
    void ReadRegisters(uint8_t regAddr, uint8_t* buffer, uint8_t len);

public:
    struct AccelData {
        int16_t x;
        int16_t y;
        int16_t z;
    };
    struct GyroData {
        int16_t x;
        int16_t y;
        int16_t z;
    };

    MPU6500(ISPI* p_spi, IGPIO* p_csPin);
    uint8_t ReadRegister(uint8_t regAddr);
    bool Init();
    AccelData GetAccel();
    GyroData GetGyro();
    void SetGyroRange(GyroRange range);
    void SetAccelRange(AccelRange range);
    void SetGyroDLPF(DLPFBandwidth bandwidth);
    void SetAccelDLPF(DLPFBandwidth bandwidth);
    void SetSampleRate(uint16_t rateHz);
    void EnableDataReadyInterrupt();

};

#endif /* SENSORS_INC_MPU6500_H_ */
