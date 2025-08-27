#ifndef __NVIC_H__
#define __NVIC_H__
#include "stm32f10x.h"

void NVIC_EnableIRQ(uint8_t IRQn, uint8_t PreemptionPriority, uint8_t SubPriority);

#endif // !__NVIC_H__
