#ifndef __MyDS1302_H__
#define __MyDS1302_H__
extern char DS1302_DateTime[]; // 日期时间设置 {年, 月，日，时，分，秒}
void DS1302_Init();
void DS1302_SetDateTime();
void DS1302_GetDateTime();
#endif // !__MyDS1302_H__
