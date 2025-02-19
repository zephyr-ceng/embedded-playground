/****************
 *@description: 中断寄存器设置,中断寄存器：T0
 *@author: zephyr
 *@date: 2025-02-03 22:16:10
 *@version: V1.0.0
 ****************/
/* 
`interrupt 0`：外部中断0（INT0）的中断服务程序。

`interrupt 1`：定时器0溢出中断的中断服务程序。

`interrupt 2`：外部中断1（INT1）的中断服务程序。

`interrupt 3`：定时器1溢出中断的中断服务程序。

`interrupt 4`：串口中断的中断服务程序。
 */

/*******
 *@description: T0中断初始化设置
 *@parameters: null
 *@returnValue: Null
 *******/
void Timer0_Init()
{
    IE   = 0x82;                // 设置ET0为1，使能定时器0中断，EA为1，启用全局中断
    TMOD = 0x01;                // 定时器0为模式1（16位定时器）0x02 13位定时器 0x03 8位自动重装定时器
    TCON |= 0x10;               // 设置TR0为1，启动定时器0 清除TF0: TCON &= ~0x10;
    TH0 = (65535 - 2000) / 256; // 定时2000ms
    TL0 = (65535 - 2000) % 256;
}

/*******
 *@description: T0中断处理函数
 *@parameters: Null
 *@returnValue: Null
*******/

void Timer0_ISR(void) interrupt 1
{
    /* code */
    TH0 = (65535 - 2000) / 256; // 定时2000ms
    TL0 = (65535 - 2000) % 256;
}


/*******
 *@description: 中断T1的初始化设置  
 *@parameters: Null
 *@returnValue: Null
*******/
void Timer1_Init()
{
    IE   = 0x88;                // 设置ET1为1，使能定时器1中断，EA为1，启用全局中断
    TMOD = 0x10;                // 定时器1为模式1（16位定时器）0x00 13位定时器 0x02 8位自动重装定时器 0x03 定时器停止模式
    TCON |= 0x40;               // 设置TR1为1，启动定时器1 清除TF1: TCON &= ~0x40;
    TH0 = (65535 - 2000) / 256; // 定时2000ms
    TL0 = (65535 - 2000) % 256;
}

/*******
 *@description: T0中断处理函数,
 *@parameters: Null
 *@returnValue: Null
*******/

void Timer1_ISR(void) interrupt 3
{
    /* code */
    TH0 = (65535 - 2000) / 256; // 定时2000ms
    TL0 = (65535 - 2000) % 256;
}

/*******
 *@description: 外部中断INT0 和INT1使用
 *@parameters: Null
 *@returnValue: Null
*******/
void External_Interrupt0_Init(void)
{
    IE = 0x81; // EA = 1 EX0 = 1
    TCON = 0x01; // IT0 = 1; 边沿触发 默认0 上升沿触发
}
/*******
 *@description: 外部中断INT0 和 INT1 中断处理函数
 *@parameters: Null
 *@returnValue: Null
*******/

void External_Interrupt0_ISR(void) interrupt 0
{
    /* code */
}