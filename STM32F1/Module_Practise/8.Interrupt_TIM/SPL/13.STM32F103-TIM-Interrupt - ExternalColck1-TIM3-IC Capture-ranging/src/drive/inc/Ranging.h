
#ifndef __Ranging_H__
#define __Ranging_H__
void GPIO_Init_Config();
void TIM3_CH_Config();
void Trigger_Ultrasonic();
int16_t Get_distance_value();
int16_t Get_captureStatus();
#endif // !__Ranging_H__

