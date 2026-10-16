/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pins : EC11_A_Pin EC11_B_Pin */
    GPIO_InitStruct.Pin = EC11_A_Pin | EC11_B_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

/* USER CODE BEGIN 2 */
/**
 * @brief 读取 EC11 编码器 A、B 两相的当前电平状态。
 * @note  返回值 bit1 对应 A 相，bit0 对应 B 相；例如 0x02 表示 A 高、B 低。
 * @retval 编码器当前的两位状态，取值范围为 0x00 到 0x03。取值说明：
    *         - 0x00: A 低，B 低
    *         - 0x01: A 低，B 高
    *         - 0x02: A 高，B 低
    *         - 0x03: A 高，B 高
 */
uint8_t EC11_ReadState(void) {
    uint8_t state = 0;

    if (HAL_GPIO_ReadPin(EC11_A_GPIO_Port, EC11_A_Pin) == GPIO_PIN_SET) {
        state |= 0x02U; // 设置A相状态为1
    }
    if (HAL_GPIO_ReadPin(EC11_B_GPIO_Port, EC11_B_Pin) == GPIO_PIN_SET) {
        state |= 0x01U; // 设置B相状态为1
    }

    return state;
}
/* USER CODE END 2 */
