/*
 * app_main.h
 *
 *  Created on: Apr 28, 2026
 *      Author: vinh
 */

#ifndef APP_MAIN_H_
#define APP_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    float ax, ay, az;
    float gx, gy, gz;
} IMUData_t;
void App_Init(void);
void App_Loop(void);
void IMU_Process(void);
void LoRa_Process(IMUData_t *data);
#ifdef __cplusplus
}
#endif


#endif /* APP_MAIN_H_ */
