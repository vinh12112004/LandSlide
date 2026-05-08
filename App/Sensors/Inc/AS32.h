/*
 * AS32.h
 *
 *  Created on: May 2, 2026
 *      Author: vinh
 */

#ifndef SENSORS_INC_AS32_H_
#define SENSORS_INC_AS32_H_

#include "../../Protocols/Inc/IGPIO.h"
#include "../../Protocols/Inc/IUART.h"

class AS32 {
public:
    enum Mode {
        MODE_NORMAL    = 0, // M0 = 0, M1 = 0
        MODE_WAKEUP    = 1, // M0 = 1, M1 = 0
        MODE_POWERSAVE = 2, // M0 = 0, M1 = 1
        MODE_SLEEP     = 3  // M0 = 1, M1 = 1
    };

private:
    IGPIO* m0;
    IGPIO* m1;
    IGPIO* aux;
    IUART* uart;

    void WaitAuxHigh();

public:
    AS32(IGPIO* p_m0, IGPIO* p_m1, IGPIO* p_aux, IUART* p_uart);
    void Init();
    void SetMode(Mode mode);
    void SendData(const uint8_t* data, uint16_t len);
};

#endif /* SENSORS_INC_AS32_H_ */
