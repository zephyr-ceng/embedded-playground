/****************
 *@description: 串口通讯主机（发送端）
 *@author: zephyr
 *@date: 2025-02-04 20:18:13
 *@version: V1.0.0
 ****************/

/* 系统头文件 */
#include "./drive/89C52.H"
#include "./drive/MyRename.h"
#include "./drive/MyDelay.h"


#define Port_LED0 P0
#define Port_LED2 P2

/*******
 *@description: 串口初始化--允许多机通讯
 *@parameters: Null
 *@returnValue: Null
 *******/
void UART_Init(void)
{
    IE   = 0x90;              // EA = 1; ES = 1; 打开总中断和串口中断
    SCON = 0x70;              // 往SCON寄存器写入01010000，SM0位=0、SM1位=1，让串行口工作在方式1，SM2=1 允许多机通讯；REN=1，允许接收数据
    PCON = 0x00;              // 往PCON寄存器写入00H，SMOD位=0，波特率保持不变
    TCON = 0x40;              // 启动Timer1
    TMOD = 0x20;              // 往TMOD寄存器写入00100000，M1位=1、M0位=0，让定时器T1工作在方式2(8位自动重装计数器)，
    TH1  = 5; // 往定时器T1的TH1寄存器写入重装值FAH(250)，将串行通讯波特率设为4.8kbit /s (晶振为11.0592MHz)
}
/*******
 *@description: 使用串口接收数据
 *@parameters: Null
 *@returnValue: Dat 接收的数据
*******/
uint8_t UART_Revive_Byte()
{
    uint8_t Dat;
    while (RI == 1);
    Dat = SBUF;
    RI = 0;
    return Dat;
}


void main()
{
    /* 模块初始化 */
    UART_Init();
    /* 主循环 */
    while (1) {
        Port_LED0 = ~UART_Revive_Byte();
        Port_LED2 = ~UART_Revive_Byte();
        // DelayMs(200);
    }
}