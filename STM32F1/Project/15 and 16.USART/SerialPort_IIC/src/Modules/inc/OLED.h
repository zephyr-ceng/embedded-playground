
#ifndef _OLED_H
#define _OLED_H

#include "stm32f10x.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t rows, uint8_t cols, char ch);
void OLED_ShowString(uint8_t rows, uint8_t cols, const char *str);
void OLED_ShowStringN(uint8_t rows, uint8_t cols, const char *str, uint8_t len);
void OLED_ShowStringScroll(uint8_t rows, uint8_t cols, const char *str, uint8_t len, uint16_t scrollTime);
void OLED_ShowSNumber(uint8_t rows, uint8_t cols, int32_t num,uint8_t width);
void OLED_ShowFloat(uint8_t rows, uint8_t cols, float num, uint8_t width, uint8_t precision);
void OLED_ShowHexNumber(uint8_t rows, uint8_t cols, uint32_t num, uint8_t width);
#endif // !_OLED_H