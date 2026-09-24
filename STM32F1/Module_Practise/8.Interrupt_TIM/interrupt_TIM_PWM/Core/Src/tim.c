/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    tim.c
 * @brief   This file provides code for the configuration
 *          of the TIM instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "tim.h"

/* USER CODE BEGIN 0 */
uint32_t overValue = 0;
/* USER CODE END 0 */

TIM_HandleTypeDef htim1;

/* TIM1 init function */
void MX_TIM1_Init(void) {
    /* USER CODE BEGIN TIM1_Init 0 */

    /* USER CODE END TIM1_Init 0 */

    /* USER CODE BEGIN TIM1_Init 1 */

    /* USER CODE END TIM1_Init 1 */

    // TIM1 配置为每500ms 触发一次中断
    // 计数器时钟 = 72 MHz / (Prescaler + 1) = 72 MHz / 7200 = 10 kHz = 0.1 ms
    // 计数器周期 = (Period + 1) / 计数器时钟 = (5000) / 10 kHz = 5000 * 0.1 ms = 500 ms
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 7200 - 1;                   // 分频器：72 MHz / 7200 = 10 kHz
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;       // 向上计数模式
    htim1.Init.Period = 5000 - 1;                      // 计数器：5000 个计数周期 = 500 ms
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 不分频，时钟直接输入
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // 自动重装载寄存器预装载功能关闭
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
        // 初始化错误处理
        Error_Handler();
    }

    /* USER CODE BEGIN TIM1_Init 2 */

    /* USER CODE END TIM1_Init 2 */
}

// TIM1 MspInit and DeInit functions are called by HAL_TIM_Base_Init and
// HAL_TIM_Base_DeInit respectively. They handle the low-level hardware
// initialization and de-initialization for TIM1, such as enabling the clock and
// configuring interrupts.
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle) {
    if (tim_baseHandle->Instance == TIM1) {
        /* USER CODE BEGIN TIM1_MspInit 0 */

        /* USER CODE END TIM1_MspInit 0 */
        /* TIM1 clock enable */
        __HAL_RCC_TIM1_CLK_ENABLE();

        /* TIM1 interrupt Init */
        HAL_NVIC_SetPriority(TIM1_UP_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
        /* USER CODE BEGIN TIM1_MspInit 1 */

        /* USER CODE END TIM1_MspInit 1 */
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle) {
    if (tim_baseHandle->Instance == TIM1) {
        /* USER CODE BEGIN TIM1_MspDeInit 0 */

        /* USER CODE END TIM1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM1_CLK_DISABLE();

        /* TIM1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM1_UP_IRQn);
        /* USER CODE BEGIN TIM1_MspDeInit 1 */

        /* USER CODE END TIM1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

// 返回当前计数器的值
uint32_t TIM_GetCurrentCounterValue(void) {
    return __HAL_TIM_GET_COUNTER(&htim1);
}

// 返回溢出计数器的值
uint32_t TIM_GetOverflowCount(void) {
    return overValue;
}

// TIM溢出中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim1) {
        overValue++;
        if (overValue >= 10) // 溢出次数达到 10 后翻转 LED，即每 500 ms 翻转一次 LED
        {
            overValue = 0;        // 重置溢出计数器
            GPIO_Toggle_Status(); // 切换LED状态
        }
    }
}

/* USER CODE END 1 */
