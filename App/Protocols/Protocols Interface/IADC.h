/*
 * IADC.h
 *
 *  Created on: May 13, 2026
 *      Author: vinh
 */

#ifndef PROTOCOLS_INC_INTERFACE_IADC_H_
#define PROTOCOLS_INC_INTERFACE_IADC_H_

#include <stdint.h>

class IADC {
public:
    virtual uint16_t Read() = 0;
    virtual ~IADC() {}
};

#endif /* PROTOCOLS_INC_INTERFACE_IADC_H_ */
