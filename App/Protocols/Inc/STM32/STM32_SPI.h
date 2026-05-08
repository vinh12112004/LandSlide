/*
 * Stm32Spi.h
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */

#ifndef PROTOCOLS_INC_STM32_STM32_SPI_H_
#define PROTOCOLS_INC_STM32_STM32_SPI_H_

#include "main.h"

#include "Interface/ISPI.h"

class STM32_SPI : public ISPI {
private:
    SPI_HandleTypeDef* hspi;

public:
    STM32_SPI(SPI_HandleTypeDef* h);
    void ReadWrite(const uint8_t* txData, uint8_t* rxData, uint16_t size) override;
};

#endif // STM32_SPI_H
