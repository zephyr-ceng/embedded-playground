/****************
 *@description: 流水灯-逐个点亮然后逐个熄灭
 *@author: zephyr
 *@date: 2025-03-26 22:29:32
 *@version: V1.0.0
****************/

#include "stm32f10x.h"
#include "./drive/MyDelay.h"

int main(void)
{
    GPIO_InitTypeDef gpio_initstruct = {0};
    // 开启端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 初始化IO 推挽模式
    gpio_initstruct.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    gpio_initstruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&gpio_initstruct); // 配置端口引脚

    while (1) {
        // 关闭灯 将端口写入高电平
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        Delay_ms(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_2);
        Delay_ms(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_3);
        Delay_ms(500);

        // 开灯 将端口写入低电平
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        Delay_ms(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_2);
        Delay_ms(500);
        GPIO_ResetBits(GPIOA, GPIO_Pin_3);
        Delay_ms(500);
    }
}
