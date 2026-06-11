/*
 * AS32.cpp
 *
 *  Created on: May 2, 2026
 *      Author: vinh
 */
#include "AS32.h"
#include "cmsis_os.h"
#include "string.h"
AS32::AS32(IGPIO* p_m0, IGPIO* p_m1, IGPIO* p_aux, IUART* p_uart)
    : m0(p_m0), m1(p_m1), aux(p_aux), uart(p_uart) {
}

void AS32::WaitAuxHigh() {
    while (aux->Read() == false) {
    	osDelay(1);
    }
}

void AS32::Init() {
    SetMode(MODE_NORMAL);
}

void AS32::SetMode(Mode mode) {
    WaitAuxHigh();
    switch (mode) {
        case MODE_NORMAL:    m0->Write(false); m1->Write(false); break;
        case MODE_WAKEUP:    m0->Write(true);  m1->Write(false); break;
        case MODE_POWERSAVE: m0->Write(false); m1->Write(true);  break;
        case MODE_SLEEP:     m0->Write(true);  m1->Write(true);  break;
    }
    WaitAuxHigh();
}

void AS32::SendData(const uint8_t* data, uint16_t len) {
    WaitAuxHigh();
    uart->Send(data, len);
    WaitAuxHigh();
}
bool AS32::SendDataIT(const uint8_t* data, uint16_t len) {
    if (isTransmitting) return false;

    WaitAuxHigh();

    if (len > sizeof(txBuffer)) len = sizeof(txBuffer);
    memcpy(txBuffer, data, len);

    isTransmitting = true;
    uart->SendIT(txBuffer, len);

    return true;
}
bool AS32::SendDataDMA(const uint8_t* data, uint16_t len) {
    if (isTransmitting) return false;

    WaitAuxHigh();

    if (len > sizeof(txBuffer)) len = sizeof(txBuffer);
    memcpy(txBuffer, data, len);

    isTransmitting = true;
    uart->SendDMA(txBuffer, len);

    return true;
}

void AS32::OnTxComplete() {
    isTransmitting = false;
}

