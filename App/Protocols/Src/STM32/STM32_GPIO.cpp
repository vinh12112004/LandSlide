/*
 * STM32_GPIO.cpp
 *
 *  Created on: May 2, 2026
 *      Author: vinh
 */
#include "../Inc/STM32/STM32_GPIO.h"

STM32_GPIO::STM32_GPIO(GPIO_TypeDef* p, uint16_t p_pin) : port(p), pin(p_pin) {
}

void STM32_GPIO::Write(bool state) {
    HAL_GPIO_WritePin(port, pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool STM32_GPIO::Read() {
    return HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET;
}
