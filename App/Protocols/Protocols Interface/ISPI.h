/*
 * ISpi.h
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */

#ifndef PROTOCOLS_INC_INTERFACE_ISPI_H_
#define PROTOCOLS_INC_INTERFACE_ISPI_H_

#include <stdint.h>

class ISPI {
public:
	virtual void ReadWrite(const uint8_t* txData, uint8_t* rxData, uint16_t size) = 0;
	virtual void ReadWriteDMA(uint8_t* txData, uint8_t* rxData, uint16_t size) = 0;
	virtual ~ISPI() {}
};



#endif /* PROTOCOLS_INC_INTERFACE_ISPI_H_ */


