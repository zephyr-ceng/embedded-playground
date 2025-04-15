#include "stm32f10x.h"                  // Device header
 
 
volatile int16_t encoderCount;
 
void encoder_init(void)
{
 
	//第一步，时钟配置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE); //开启RCC时钟
	//EXTI和NVIC两个外设的时钟是一直开的 ，NVIC内核外设都是不需要开启时钟
 
	//第二步，配置GPIO
	//首先定义结构体
	GPIO_InitTypeDef GPIO_initstruct;  //结构体名字GPIO_initstruct
	//将结构体成员引出来
	//对于EXTI来说，模式为浮空输入|上拉输入|下拉输入；不知该写什么模式，可以看参考手册中的外设GPIO配置
	GPIO_initstruct.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
	GPIO_initstruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	//最后初始化GPIO
	GPIO_Init(GPIOB,&GPIO_initstruct);	//传地址
	
	//第三步，配置AFIO外设中断引脚选择
	//AFIO的库函数是和GPIO在一个文件里，可以查看Library文件中的gpio.h查看函数
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);
	//第四步，配置EXTI,这样PB14的电平信号就能够通过EXTI通向下一级的NVIC了
	EXTI_InitTypeDef EXTI_InitStructure;//结构体类型名EXTI_InitTypeDef，变量名EXTI_InitStructure
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//因为上面是GPIO_Mode_IPU设置为高电平，所以触发中断是下降
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	
	//第五步，配置NVIC，NVIC是内核外设，所以它的库函数在misc.h
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //分组方式，整个芯片只能用一种。如放在模块中进行分组，要确保每个模块分组都选的是同一个；或者将这个代码放在主函数的最开始
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);	
}
 
 
// EXTI0中断服务函数（A相）
void EXTI0_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        // 读取当前A、B相状态
        uint8_t a_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
        uint8_t b_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
        
        // 判断方向
        if(a_state == b_state) {
            encoderCount++;  // 顺时针
        } else {
            encoderCount--;  // 逆时针
        }
        
        EXTI_ClearITPendingBit(EXTI_Line0);  // 清除中断标志
    }
}

// EXTI1中断服务函数（B相）
void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        // 读取当前A、B相状态
        uint8_t a_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
        uint8_t b_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
        
        // 判断方向
        if(a_state != b_state) {
            encoderCount++;  // 顺时针
        } else {
            encoderCount--;  // 逆时针
        }
        
        EXTI_ClearITPendingBit(EXTI_Line1);  // 清除中断标志
    }
}
 