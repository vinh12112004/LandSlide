/*
 * STM32_UART.h
 *
 *  Created on: May 2, 2026
 *      Author: vinh
 */

#ifndef PROTOCOLS_INC_STM32_STM32_UART_H_
#define PROTOCOLS_INC_STM32_STM32_UART_H_

#include "main.h"

#include "IUART.h"

class STM32_UART : public IUART {
private:
    UART_HandleTypeDef* huart;

public:
    STM32_UART(UART_HandleTypeDef* h);
    void Send(const uint8_t* data, uint16_t len) override;
    bool Receive(uint8_t* buffer, uint16_t len, uint32_t timeout) override;
};

#endif /* PROTOCOLS_INC_STM32_STM32_UART_H_ */
