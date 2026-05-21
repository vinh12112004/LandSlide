/*
 * STM32_UART.cpp
 *
 *  Created on: May 2, 2026
 *      Author: vinh
 */
#include "STM32_UART.h"

STM32_UART::STM32_UART(UART_HandleTypeDef* h) : huart(h) {
}

void STM32_UART::Send(const uint8_t* data, uint16_t len) {
    HAL_UART_Transmit(huart, (uint8_t*)data, len, HAL_MAX_DELAY);
}

bool STM32_UART::Receive(uint8_t* buffer, uint16_t len, uint32_t timeout) {
    return HAL_UART_Receive(huart, buffer, len, timeout) == HAL_OK;
}

void STM32_UART::ReceiveIT(uint8_t* data, uint16_t len)
{
    HAL_UART_Receive_IT(huart, data, len);
}
