/****************
 *@description: LDR 光敏传感器
 *@author: zephyr
 *@date: 2025-04-02 22:01:18
 *@version: V1.0.0
****************/

#ifndef __LDR_H__
#define __LDR_H__

void LDR_Init();
uint16_t Read_Light_Sensor();
void LDR_Digital_Init();
uint8_t Read_Light_Sensor_Digital();

#endif // !__LDR_H__
