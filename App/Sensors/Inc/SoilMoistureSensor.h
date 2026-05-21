/*
 * SoilMoistureSensor.h
 *
 *  Created on: May 13, 2026
 *      Author: vinh
 */

#ifndef SENSORS_INC_SOILMOISTURESENSOR_H_
#define SENSORS_INC_SOILMOISTURESENSOR_H_

#include <stdint.h>
#include "IADC.h"

class SoilMoistureSensor {
private:
    IADC* adc;

    uint16_t dryValue;
    uint16_t wetValue;
    float vref;
    uint16_t adcMax;

public:
    SoilMoistureSensor(
        IADC* p_adc,
        uint16_t dry = 3000,
        uint16_t wet = 1200,
        float referenceVoltage = 3.3f,
        uint16_t adcResolution = 4095
    );

    uint16_t ReadRaw();
    uint16_t ReadRawAverage(uint8_t samples);

    float ReadVoltage();
    float ReadMoisturePercent();

    void SetCalibration(uint16_t dry, uint16_t wet);
};

#endif /* SENSORS_INC_SOILMOISTURESENSOR_H_ */
