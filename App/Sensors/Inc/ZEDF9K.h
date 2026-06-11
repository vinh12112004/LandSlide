/*
 * ZEDF9K.h
 *
 *  Created on: May 19, 2026
 *      Author: vinh
 */

#ifndef SENSORS_INC_ZEDF9K_H_
#define SENSORS_INC_ZEDF9K_H_

#include <stdint.h>
#include <stdbool.h>
#include "IUART.h"
#define GPS_RX_BUFFER_SIZE 512
class ZEDF9K {
public:
	struct NavPvtData {
	    uint8_t fixType;
	    uint8_t numSV;

	    double lat;
	    double lon;

	    float hMSL;
	    float hAcc;
	    float vAcc;
	    float pDOP;
	};

    uint8_t rxBuffer[GPS_RX_BUFFER_SIZE];


private:
    IUART* uart;
    uint16_t oldPos;
    enum ParseState {
        WAIT_SYNC1,
        WAIT_SYNC2,
        READ_CLASS,
        READ_ID,
        READ_LEN1,
        READ_LEN2,
        READ_PAYLOAD,
        READ_CK_A,
        READ_CK_B
    };

    ParseState state;

    uint8_t msgClass;
    uint8_t msgId;
    uint16_t payloadLen;
    uint16_t payloadIndex;

    uint8_t payload[100];

    uint8_t ckA;
    uint8_t ckB;
    uint8_t rxCkA;
    uint8_t rxCkB;

    NavPvtData navData;
    bool navPvtUpdated;


private:
    void ResetParser();
    void UpdateChecksum(uint8_t b);
    void ParseNavPvt();

    static uint16_t ReadU2(const uint8_t* p);
    static uint32_t ReadU4(const uint8_t* p);
    static int32_t ReadI4(const uint8_t* p);

    void SendUbx(uint8_t cls, uint8_t id, const uint8_t* payload, uint16_t len);

public:
    ZEDF9K(IUART* p_uart);

    void Init();

    void EnableNavPvtUART1();
    void DisableNMEA_UART1();

    void ProcessByte(uint8_t byte);

    bool HasNewNavPvt();
    NavPvtData GetNavPvt();

    bool IsGpsValid();

    void StartReceive();
	void OnRxByte();

	void ProcessCircularBuffer(uint16_t currentPos);
	void ResetDMAIndex();
};

#endif /* SENSORS_INC_ZEDF9K_H_ */
