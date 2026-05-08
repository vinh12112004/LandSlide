/*
 * IUART.h
 *
 *  Created on: May 2, 2026
 *      Author: vinh
 */

#ifndef PROTOCOLS_INC_INTERFACE_IUART_H_
#define PROTOCOLS_INC_INTERFACE_IUART_H_

#include <stdint.h>

class IUART {
public:
    virtual void Send(const uint8_t* data, uint16_t len) = 0;
    virtual bool Receive(uint8_t* buffer, uint16_t len, uint32_t timeout) = 0;
    virtual ~IUART() {}
};

#endif /* PROTOCOLS_INC_INTERFACE_IUART_H_ */
