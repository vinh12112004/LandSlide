#ifndef LORA_GATEWAY_H
#define LORA_GATEWAY_H

#include <Arduino.h>
#include "LoraPacket.h"

enum LoRaMode {
    NORMAL,
    WAKEUP,
    POWERSAVE,
    CONFIG
};

void Lora_Init();
bool Lora_ReadPacket(PacketData_t *packet);
void Lora_PrintPacket(const PacketData_t &packet);

#endif