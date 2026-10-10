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
#include "tim.h"
#include <stdint.h>

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

volatile uint32_t risingEdge = 0;
volatile uint32_t fallingEdge = 0;
// 下降沿中断确认已捕获完整脉冲后置 1，由主循环在下一次测量前清零。
volatile uint8_t captureComplete = 0;

TIM_HandleTypeDef htim1;

/* TIM1 init function */
void MX_TIM1_Init(void) {

    /* USER CODE BEGIN TIM1_Init 0 */

    /* USER CODE END TIM1_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};

    /* USER CODE BEGIN TIM1_Init 1 */

    /* USER CODE END TIM1_Init 1 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 72 - 1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 65535;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }
    // 配置时钟源为内部时钟
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_IC_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    // 配置输入捕获通道1和通道2
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;
    if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
    if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM1_Init 2 */

    /* USER CODE END TIM1_Init 2 */
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (tim_baseHandle->Instance == TIM1) {
        /* USER CODE BEGIN TIM1_MspInit 0 */

        /* USER CODE END TIM1_MspInit 0 */
        /* TIM1 clock enable */
        __HAL_RCC_TIM1_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    */
        GPIO_InitStruct.Pin = ECHO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(ECHO_GPIO_Port, &GPIO_InitStruct);

        /* TIM1 interrupt Init */
        HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
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

        /**TIM1 GPIO Configuration
    PA8     ------> TIM1_CH1
    */
        HAL_GPIO_DeInit(ECHO_GPIO_Port, ECHO_Pin);

        /* TIM1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM1_CC_IRQn);
        /* USER CODE BEGIN TIM1_MspDeInit 1 */

        /* USER CODE END TIM1_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    // 仅在 TIM1 的下降沿中断中处理，并确认此前已经捕获到上升沿。
    if ((htim->Instance == TIM1) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        && (captureComplete == 0U) && (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_CC1) != RESET)) {
        // CH1 无中断，在 CH2 下降沿中断中读取两个硬件捕获值。
        risingEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
        fallingEdge = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
        captureComplete = 1;
    }
}

uint32_t return_distance_cm(void) {
    // 16 位无符号差值可自然处理一次计数器回绕；当前计数频率为 1 MHz，即每计数 1 us。
    uint16_t pulseWidthUs = (uint16_t)(fallingEdge - risingEdge);
    // 不完整、零宽度或超过 40 ms 的脉冲视为无效，避免缺失边沿导致无符号下溢后显示异常大距离。
    if ((captureComplete == 0U) || (pulseWidthUs == 0U) || (pulseWidthUs > 40000U)) {
        return 0U;
    }
    // 距离(cm) = 时间(us) * 声速（cm/us） / 2，按 340m/s换算为0.034 并四舍五入。
    return (uint32_t)pulseWidthUs * 0.034f / 2.0f + 0.5f; // +0.5f 四舍五入
}
/* USER CODE END 1 */
