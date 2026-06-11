/*
 * STM32_ADC.h
 *
 *  Created on: May 13, 2026
 *      Author: vinh
 */

#ifndef PROTOCOLS_INC_STM32_STM32_ADC_H_
#define PROTOCOLS_INC_STM32_STM32_ADC_H_

#include "main.h"
#include "IADC.h"

class STM32_ADC : public IADC {
private:
    ADC_HandleTypeDef* hadc;

public:
    STM32_ADC(ADC_HandleTypeDef* h);
    uint16_t Read() override;
    void ReadDMA(uint16_t* buffer, uint16_t length) override;
    void StopDMA() override;
};

#endif /* PROTOCOLS_INC_STM32_STM32_ADC_H_ */
