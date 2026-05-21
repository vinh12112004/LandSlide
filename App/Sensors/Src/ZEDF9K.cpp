/*
 * ZEDF9K.cpp
 *
 *  Created on: May 19, 2026
 *      Author: vinh
 */
#include "ZEDF9K.h"
#include "cmsis_gcc.h"
ZEDF9K::ZEDF9K(IUART* p_uart)
    : uart(p_uart) {
    ResetParser();
    navPvtUpdated = false;
}

void ZEDF9K::Init() {
    EnableNavPvtUART1();
    StartReceive();
}

void ZEDF9K::StartReceive()
{
    uart->ReceiveIT(&rxByte, 1);
}

void ZEDF9K::OnRxByte()
{
    ProcessByte(rxByte);
    StartReceive();
}

void ZEDF9K::ResetParser() {
    state = WAIT_SYNC1;
    msgClass = 0;
    msgId = 0;
    payloadLen = 0;
    payloadIndex = 0;
    ckA = 0;
    ckB = 0;
    rxCkA = 0;
    rxCkB = 0;
}

void ZEDF9K::UpdateChecksum(uint8_t b) {
    ckA = ckA + b;
    ckB = ckB + ckA;
}

uint16_t ZEDF9K::ReadU2(const uint8_t* p) {
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

uint32_t ZEDF9K::ReadU4(const uint8_t* p) {
    return (uint32_t)p[0]
        | ((uint32_t)p[1] << 8)
        | ((uint32_t)p[2] << 16)
        | ((uint32_t)p[3] << 24);
}

int32_t ZEDF9K::ReadI4(const uint8_t* p) {
    return (int32_t)ReadU4(p);
}

void ZEDF9K::SendUbx(uint8_t cls, uint8_t id, const uint8_t* pl, uint16_t len) {
    uint8_t header[6];

    header[0] = 0xB5;
    header[1] = 0x62;
    header[2] = cls;
    header[3] = id;
    header[4] = len & 0xFF;
    header[5] = (len >> 8) & 0xFF;

    uint8_t a = 0;
    uint8_t b = 0;

    for (int i = 2; i < 6; i++) {
        a += header[i];
        b += a;
    }

    for (uint16_t i = 0; i < len; i++) {
        a += pl[i];
        b += a;
    }

    uart->Send(header, 6);

    if (len > 0) {
        uart->Send(pl, len);
    }

    uint8_t checksum[2] = {a, b};
    uart->Send(checksum, 2);
}

void ZEDF9K::EnableNavPvtUART1() {
    /*
     * UBX-CFG-MSG
     * Enable UBX-NAV-PVT on UART1
     *
     * Payload:
     * msgClass = 0x01
     * msgID    = 0x07
     * rateI2C  = 0
     * rateUART1= 1
     * rateUART2= 0
     * rateUSB  = 0
     * rateSPI  = 0
     * reserved = 0
     */

    uint8_t payload[8] = {
        0x01, 0x07,
        0x00,
        0x01,
        0x00,
        0x00,
        0x00,
        0x00
    };

    SendUbx(0x06, 0x01, payload, 8);
}

void ZEDF9K::DisableNMEA_UART1() {
    /*
     * Tắt một số NMEA phổ biến trên UART1:
     * GGA, GLL, GSA, GSV, RMC, VTG
     */

    uint8_t nmeaMsgs[][2] = {
        {0xF0, 0x00}, // GGA
        {0xF0, 0x01}, // GLL
        {0xF0, 0x02}, // GSA
        {0xF0, 0x03}, // GSV
        {0xF0, 0x04}, // RMC
        {0xF0, 0x05}  // VTG
    };

    for (int i = 0; i < 6; i++) {
        uint8_t payload[8] = {
            nmeaMsgs[i][0],
            nmeaMsgs[i][1],
            0x00,
            0x00, // UART1 off
            0x00,
            0x00,
            0x00,
            0x00
        };

        SendUbx(0x06, 0x01, payload, 8);
    }
}

void ZEDF9K::ProcessByte(uint8_t byte) {
    switch (state) {
        case WAIT_SYNC1:
            if (byte == 0xB5) {
                state = WAIT_SYNC2;
            }
            break;

        case WAIT_SYNC2:
            if (byte == 0x62) {
                ckA = 0;
                ckB = 0;
                state = READ_CLASS;
            } else {
                state = WAIT_SYNC1;
            }
            break;

        case READ_CLASS:
            msgClass = byte;
            UpdateChecksum(byte);
            state = READ_ID;
            break;

        case READ_ID:
            msgId = byte;
            UpdateChecksum(byte);
            state = READ_LEN1;
            break;

        case READ_LEN1:
            payloadLen = byte;
            UpdateChecksum(byte);
            state = READ_LEN2;
            break;

        case READ_LEN2:
            payloadLen |= ((uint16_t)byte << 8);
            UpdateChecksum(byte);

            if (payloadLen > sizeof(payload)) {
                ResetParser();
            } else {
                payloadIndex = 0;
                state = READ_PAYLOAD;
            }
            break;

        case READ_PAYLOAD:
            payload[payloadIndex++] = byte;
            UpdateChecksum(byte);

            if (payloadIndex >= payloadLen) {
                state = READ_CK_A;
            }
            break;

        case READ_CK_A:
            rxCkA = byte;
            state = READ_CK_B;
            break;

        case READ_CK_B:
            rxCkB = byte;

            if (rxCkA == ckA && rxCkB == ckB) {
                if (msgClass == 0x01 && msgId == 0x07) {
                    ParseNavPvt();
                }
            }

            ResetParser();
            break;
    }
}

void ZEDF9K::ParseNavPvt() {
    if (payloadLen < 92) {
        return;
    }

    navData.fixType = payload[20];
    navData.numSV   = payload[23];

    int32_t lonRaw = ReadI4(&payload[24]);
    int32_t latRaw = ReadI4(&payload[28]);

    int32_t hMSLRaw = ReadI4(&payload[36]);
    uint32_t hAccRaw = ReadU4(&payload[40]);
    uint32_t vAccRaw = ReadU4(&payload[44]);

    uint16_t pDOPRaw = ReadU2(&payload[76]);

    navData.lon = lonRaw * 1e-7;
    navData.lat = latRaw * 1e-7;

    navData.hMSL = hMSLRaw / 1000.0f;
    navData.hAcc = hAccRaw / 1000.0f;
    navData.vAcc = vAccRaw / 1000.0f;

    navData.pDOP = pDOPRaw * 0.01f;

    navPvtUpdated = true;
}

bool ZEDF9K::HasNewNavPvt() {
    return navPvtUpdated;
}

ZEDF9K::NavPvtData ZEDF9K::GetNavPvt()
{
    NavPvtData temp;

    __disable_irq();

    temp = navData;
    navPvtUpdated = false;

    __enable_irq();

    return temp;
}

bool ZEDF9K::IsGpsValid() {
    return (
        navData.fixType >= 3 &&
        navData.numSV >= 8 &&
        navData.hAcc < 1.0f &&
        navData.pDOP < 3.0f
    );
}



