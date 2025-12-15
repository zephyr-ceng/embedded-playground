#ifndef __ULTRASONIC_RANGING_H__
#define __ULTRASONIC_RANGING_H__

#include "stm32f10x.h"
#include "./driver/inc/MyDelay.h"


#define TRIG_PIN GPIO_Pin_9
#define ECHO_PIN GPIO_Pin_8
#define GPIO_PORT GPIOB

extern uint16_t _micros_Overflow;
void Ultrasonic_Ranging_Init();
float Ultrasonic_Ranging_GetDistance();

#endif // !__ULTRASONIC_RANGING_H__
