/*
 * IGPIO.h
 *
 *  Created on: May 2, 2026
 *      Author: vinh
 */

#ifndef PROTOCOLS_INC_INTERFACE_IGPIO_H_
#define PROTOCOLS_INC_INTERFACE_IGPIO_H_

class IGPIO {
public:
    virtual void Write(bool state) = 0;
    virtual bool Read() = 0;
    virtual ~IGPIO() {}
};

#endif /* PROTOCOLS_INC_INTERFACE_IGPIO_H_ */
