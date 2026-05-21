/*
 * STM32_ADC.cpp
 *
 *  Created on: May 13, 2026
 *      Author: vinh
 */

#include "STM32_ADC.h"

STM32_ADC::STM32_ADC(ADC_HandleTypeDef* h) : hadc(h) {
}

uint16_t STM32_ADC::Read() {
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY);

    uint16_t value = HAL_ADC_GetValue(hadc);

    HAL_ADC_Stop(hadc);

    return value;
}


