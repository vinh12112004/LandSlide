#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <Arduino.h>
#include "LoraPacket.h"

void Wifi_Connect();
void Time_Sync();
void MQTT_Init();
void MQTT_Loop();
void MQTT_PublishSensor(const PacketData_t &packet);

#endif