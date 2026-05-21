/*
 * SoilMoistureSensor.cpp
 *
 *  Created on: May 13, 2026
 *      Author: vinh
 */

#include "SoilMoistureSensor.h"

SoilMoistureSensor::SoilMoistureSensor(
    IADC* p_adc,
    uint16_t dry,
    uint16_t wet,
    float referenceVoltage,
    uint16_t adcResolution
) : adc(p_adc),
    dryValue(dry),
    wetValue(wet),
    vref(referenceVoltage),
    adcMax(adcResolution) {
}

uint16_t SoilMoistureSensor::ReadRaw() {
    return adc->Read();
}

uint16_t SoilMoistureSensor::ReadRawAverage(uint8_t samples) {
    if (samples == 0) samples = 1;

    uint32_t sum = 0;

    for (uint8_t i = 0; i < samples; i++) {
        sum += adc->Read();
    }

    return sum / samples;
}

float SoilMoistureSensor::ReadVoltage() {
    uint16_t raw = ReadRawAverage(10);
    return (raw * vref) / adcMax;
}

float SoilMoistureSensor::ReadMoisturePercent() {
    uint16_t raw = ReadRawAverage(10);

    if (raw >= dryValue) return 0.0f;
    if (raw <= wetValue) return 100.0f;

    float percent = ((float)(dryValue - raw) / (dryValue - wetValue)) * 100.0f;

    return percent;
}

void SoilMoistureSensor::SetCalibration(uint16_t dry, uint16_t wet) {
    dryValue = dry;
    wetValue = wet;
}


