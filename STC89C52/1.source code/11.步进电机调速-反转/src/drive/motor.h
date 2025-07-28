#include "89C52.h"
#include "MyRename.h"
#include "MyDelay.h"

// // 定义控制步进电机的4个引脚
// sbit IN1 = P1^0;
// sbit IN2 = P1^1;
// sbit IN3 = P1^2;
// sbit IN4 = P1^3;

#define Port1 P1

extern uint8_t speed_delay  = 1000; // 初始速度延时（500ms）
uint8_t step_Motor[] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09};

// 顺时针转动电机
void forward()
{
    unsigned char i;
    for (i = 1; i <= 8; i++) {
        Port1 = step_Motor[i]; // 按顺时针方向控制步进电机
        DelayMs(speed_delay);  // 延时控制速度
    }
}

// 逆时针转动电机
void backward()
{
    unsigned char i;
    for (i = 8; i >= 1; i--) {
        Port1 = step_Motor[i]; // 按逆时针方向控制步进电机
        DelayMs(speed_delay);  // 延时控制速度
    }
}

// 停止电机
void stop_motor()
{
    Port1 = 0x00; // 停止步进电机的转动
}

// 加速电机
void accelerate()
{
    if (speed_delay > 100) {
        speed_delay -= 50; // 减小延时，增加转速
    }
}

// 减速电机
void decelerate()
{
    if (speed_delay < 1500) {
        speed_delay += 50; // 增加延时，减小转速
    }
}
