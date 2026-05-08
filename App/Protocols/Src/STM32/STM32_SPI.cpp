/*
 * Stm32Spi.cpp
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */
#include "../Inc/STM32/STM32_SPI.h"

STM32_SPI::STM32_SPI(SPI_HandleTypeDef* h) : hspi(h) {
}

void STM32_SPI::ReadWrite(const uint8_t* txData, uint8_t* rxData, uint16_t size) {
    HAL_SPI_TransmitReceive(hspi, (uint8_t*)txData, rxData, size, HAL_MAX_DELAY);
}


