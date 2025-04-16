# STM32F103C8T6

## STM32引脚图


![引脚](assets/引脚.png)



## 系统架构

**系统结构：四个驱动单元（DC Code总线Dbus, 系统总线Sbus，DMA1, DMA2）四个被动单元（SRAM,内部闪存存储器，FSMC, AHB到APB桥--连接APB设备）**

**互联网结构：五个驱动单元（D-bus, Sbus,DMA1, DMA2, 以太网DMA），三个被动单元（SRAM,内部闪存存储器，AHB到APB桥--连接APB设备）**

![image-20250303182305574](assets/image-20250303182305574.png)

## GPIO

1. ### 由上图可知：GPIO通过APB2总线连接，采用32位编码，高16位未使用，其基本结构为

   ![image-20250303183335931](assets/image-20250303183335931.png)

2. ### GPIO的8种端口模式

   **对应 “ gpio.h ” 文件中：**

   ```C
   typedef enum
   { 
     GPIO_Mode_AIN = 0x00, // 模拟输入，GPIO无效，引脚直接接入内部ADC
     GPIO_Mode_IN_FLOATING = 0x04,// 浮空输入，可读取引脚电平，若引脚悬空，则电平不确定
     GPIO_Mode_IPD = 0x28, // 下拉输入，可读取引脚电平，内部连接下拉电阻，悬空时默认低电平
     GPIO_Mode_IPU = 0x48, // 上拉输入，可读取引脚电平，内部连接上拉电阻，悬空时默认高电平
     GPIO_Mode_Out_OD = 0x14, // 开漏输出，高电平无驱动，低电平有输出驱动能力
     GPIO_Mode_Out_PP = 0x10, // 推挽输出，高低电平均有驱动能力
     GPIO_Mode_AF_OD = 0x1C, // 复用开漏输出，可输出引脚电平，高电平为高阻态，低电平接VSS
     GPIO_Mode_AF_PP = 0x18 // 复用推挽输出，可输出引脚电平，高电平接VDD,低电平接VSS
   }GPIOMode_TypeDef;
   ```

3. LED

    - 需要了解GPIO的输入和输出

    - 常用函数

      ```C
      // 时钟使能
      void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState); // APB2内部时钟
      void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState); // APB1时钟
      void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState); // AHB时钟使能
      
      // GPIO初始化
      GPIO_InitTypeDef gpio_initstruct; // 结构体
      gpio_initstruct.GPIO_Pin   = GPIO_Pin_1; // 引脚
      gpio_initstruct.GPIO_Mode  = GPIO_Mode_Out_PP; // 输出模式
      gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz; // 速度
      void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct) // 初始化GPIO
      
      // 引脚设置
      void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); // 高电平
      void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); // 低电平
      void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal); // BitVal 值： bit_Set高或者bit_ReSet低， 
      void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal); // 控制多个端口
      ```

4. 点灯

   ```C
   // 需要三步：
   // 1.开启端口时钟
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 启用或禁用高速APB（APB 2）外围设备时钟。
   
   // 2.初始化GPIO
   GPIO_InitTypeDef gpio_initstruct; // 结构体初始化
   gpio_initstruct.GPIO_Pin   = GPIO_Pin_1;
   gpio_initstruct.GPIO_Mode  = GPIO_Mode_Out_PP; // 推挽输出
   gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
   while(1)
   {
       // 3.设置端口值
       GPIO_SetBits(GPIOA, GPIO_Pin_1); // 置高电平
       Delay_ms(200);
       GPIO_ResetBits(GPIOA, GPIO_Pin_1);// 置低电平
   }
   ```

5. GPIO端口检测
    ```C
    uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); // 端口输入检测
    uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx); // 多端口输入检测
    uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin); // 端口输出检测
    uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx); // 多端口输出检测
    ```

6. EXTI外部中断流程和相关函数

    ![image-20250416193401902](assets/image-20250416193401902.png)

    - 基本概念：STM32支持19个外部中断，0~15为IO端口输入中断，16-PVD电压监测, 17-RTC闹钟,18-USB唤醒，19-以太网端口（互联网性）
    - 0~15个IO端口分配至EXTI0~EXTI15，为了对应线和中断所以需要做映射：`void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource)`
    - 映射后设置中断的初始化，分别包含标志位（EXTI_Line）、模式选择（EXTI_Mode）、触发方式(上升沿、下降沿、双边沿)、中断有效控制，`void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);`
    - 中断可能同时触发，所以需要设置优先级（NVIC）

    

    ```C
    /* EXTI 外部中断流程
    1.使能APB2时钟（GPIO和AFIO）
    2.GPIO初始化
    3.映射GPIO至AFIO
    4.中断初始化
    5.NVIC优先级配置
    */
    
    
    ```

    

    ```C
    // 1. 使能相关端口时钟 & AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    
    // 2. GPIO初始化
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2; // PA0(A相), PA1(B相), PA2(按键)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入(根据硬件设计选择)
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 3. 映射GPIO和AFIO
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0); // PA0 -> EXTI0
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2); // PA2 -> EXTI2
    
    // 4. EXTI外部中断初始化
    EXTI_InitTypeDef EXTI_InitStructure = {0};
    
    // 编码器A相(PA0)配置：双边沿触发
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 双边沿
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    // 编码器按键(PA2)配置：下降沿触发
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 按键按下时下降沿
    EXTI_Init(&EXTI_InitStructure);
    
    // 5. 配置NVIC中断优先级
    NVIC_InitTypeDef NVIC_InitStructure = {0};
    // EXTI0中断(编码器A相)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init(&NVIC_InitStructure);
    
    // EXTI2中断(编码器按键)
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_Init(&NVIC_InitStructure);
    
    // 6. 编写中断子程序
    // EXTI0中断服务函数 - 处理编码器旋转
    void EXTI0_IRQHandler(void) {
        if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
            // 读取B相状态判断方向
            uint8_t b_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
            if(b_state) {
                encoderCount--; // B相为高时逆时针
            } else {
                encoderCount++; // B相为低时顺时针
            }
            EXTI_ClearITPendingBit(EXTI_Line0); // 清除中断标志
        }
    }
    
    // EXTI2中断服务函数 - 处理编码器按键
    void EXTI2_IRQHandler(void) {
        if(EXTI_GetITStatus(EXTI_Line2) != RESET) {
            encoderButtonPressed = 1;
            EXTI_ClearITPendingBit(EXTI_Line2);
        }
    }
    ```

7. 定时器

    1. 定时器分类
    2. 时基单元工作流程
        - 进入预分频器（PSC）,对内部的72MHz的时钟进行分频，实际的分频值 = 预分频值 + 1
        - 计数器计数，分为向上计数，向下计数，中央对齐计数
        - 自动重装初始值

    | 编号       | 类型       | 总线 | 功能                                                         |
    | ---------- | ---------- | ---- | ------------------------------------------------------------ |
    | TIM1、8    | 高级定时器 | APB2 | 拥有通用定时器全部功能，并额外具有重复计数器、死区生成、互补输出、刹车输入等功能 |
    | TIM2,3,4,5 | 通用定时器 | APB1 | 拥有基本定时器全部功能，并额外具有内外时钟源选择、输入捕获、输出比较、编码器接口、主从触发模式等功能 |
    | TIM6,7     | 基本定时器 | APB1 | 拥有定时中断、主模式触发DAC的功能                            |


8. 外设
   - EC11 编码器
   - 原理：