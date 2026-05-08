/*
 * STM32_GPIO.h
 *
 *  Created on: May 2, 2026
 *      Author: vinh
 */

#ifndef PROTOCOLS_INC_STM32_STM32_GPIO_H_
#define PROTOCOLS_INC_STM32_STM32_GPIO_H_

#include "main.h"

#include "Interface/IGPIO.h"

class STM32_GPIO : public IGPIO {
private:
    GPIO_TypeDef* port;
    uint16_t pin;

public:
    STM32_GPIO(GPIO_TypeDef* p, uint16_t p_pin);
    void Write(bool state) override;
    bool Read() override;
};

#endif /* PROTOCOLS_INC_STM32_STM32_GPIO_H_ */
