## 旋转编码器-OLED 数字加减

1.  使用了 OLED 做检测显示
2.  GPIO 上拉输入 `GPIO_Mode_IPU`
3.  GPIO 输入/出检测`uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)`/
`uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);`
4.  EXTI 中断使用

    ```c
        // 3.AFIO映射（将GPIO端口和中断关联）
        GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);
        // GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource1);

        // 4.中断初始化
        EXTI_InitTypeDef EXTI_InitStructure;
        EXTI_InitStructure.EXTI_Line    = EXTI_Line0;
        EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; // 因为上面是GPIO_Mode_IPU设置为高电平，所以触发中断是下降
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStructure);
    ```

5.  NVIC 中断优先级配置

    ```c
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 分组方式，整个芯片只能用一种。如放在模块中进行分组，要确保每个模块分组都选的是同一个；或者将这个代码放在主函数的最开始
        NVIC_InitTypeDef NVIC_InitStructure; // 结构体初始化
        NVIC_InitStructure.NVIC_IRQChannel                   = EXTI0_IRQn; // 对应中断
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;
        NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
        NVIC_Init(&NVIC_InitStructure);
    ```

6.  EC11 旋转编码器的使用原理（旋转开始产生上升沿，结束产生下降沿）
    ```c
    /* 顺时针：A 上升沿 B 低电平 A 下降沿 B 高电平
     逆时针：A 上升沿，B 高电平 A 下降沿 B 低电平 */
    // 使用双边沿检查

    uint16_t CW_Count; // 计数器
    void EXTI0_IRQHandler(void) {
        if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
        static uint8_t lastA = 0;
        //static uint8_t lastB = 0;
        uint8_t currentA = GPIO_ReadInputDataBit(EC11_PORT, EC11_A_PIN); 
        uint8_t currentB = GPIO_ReadInputDataBit(EC11_PORT, EC11_B_PIN);
                // 状态机判断方向（需结合A、B相变化）
                if (lastA == 0 && currentA == 1) {  // A上升沿
                    currentB == 0 ? CW_Count++: CCW_Count++;  // B=0:CW, B=1:CCW
                }
                else if (lastA == 1 && currentA == 0) {  // A下降沿
                    currentB == 1 ? CW_Count++:CCW_Count++;  // B=1:CW, B=0:CCW
                }

                lastA = currentA;
                // lastB = currentB;
                EXTI_ClearITPendingBit(EXTI_Line0);  // 清除中断标志
            }
    }

    // 转一次会触发两次计数
    //TODO: CW_Count每次会加2, 需优化
    ```
