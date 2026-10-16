/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* 常见正交编码器转过一个机械档位时，TIM1 在 TI12 模式下计数约 4 次。 */
#define ENCODER_COUNTS_PER_STEP 4
#define PWM_DUTY_MAX 100

/* 保存上次计数值、未凑满一个档位的计数余量，以及当前 PWM 占空比。 */
static uint16_t encoder_previous_count;
static int32_t encoder_count_remainder;
static uint8_t led_duty_percent[2] = {50U, 50U};
/* selected_led 为数组下标：0 表示 LED1，1 表示 LED2。 */
static uint8_t selected_led;
static GPIO_PinState key_stable_state;
static uint32_t key_state_change_tick;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM1_Init();
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */
    /* 清零编码器计数器，并初始化软件状态。 */
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    encoder_previous_count = 0;  // 保存上次计数值，供下次计算差值使用。
    encoder_count_remainder = 0; // 累加计数变化，每满一个编码器档位就将亮度调整 1%。
    selected_led = 0;
    key_stable_state = HAL_GPIO_ReadPin(ENCODER_KEY_GPIO_Port, ENCODER_KEY_Pin);
    key_state_change_tick = HAL_GetTick(); // 记录按键状态变化的时间戳，用于软件消抖。

    /* 同时启动 TIM1 的 CH1/CH2，开始读取编码器 A/B 相。 */
    if (HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL) != HAL_OK) {
        Error_Handler();
    }

    /* 初始化 OLED，并显示初始 PWM 占空比。 */
    OLED_Init();
    OLED_ShowString(1, 1, "LED1 Duty:");
    OLED_ShowNum(2, 1, led_duty_percent[0], 3);
    OLED_ShowChar(2, 4, '%');
    OLED_ShowString(3, 1, "TIM1 CNT:");
    OLED_ShowNum(3, 10, 0, 5);

    /* 主板 LED 为低电平点亮，比较值使用 100%-亮度进行反向映射。 */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, PWM_DUTY_MAX - led_duty_percent[0]);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, PWM_DUTY_MAX - led_duty_percent[1]);
    if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4) != HAL_OK) {
        Error_Handler();
    }

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        /* 按键低电平有效；状态与上次稳定状态不同时，等待 20 ms 消抖。 然后切换LED控制*/
        GPIO_PinState key_read_state = HAL_GPIO_ReadPin(ENCODER_KEY_GPIO_Port, ENCODER_KEY_Pin);
        uint32_t now = HAL_GetTick();
        if (key_read_state == key_stable_state) {
            key_state_change_tick = now;
        } else if (now - key_state_change_tick >= 20U) {
            key_stable_state = key_read_state;
            if (key_stable_state == GPIO_PIN_RESET) {
                selected_led ^= 1U; // 按下取反，0->1，1->0
                encoder_count_remainder = 0; // 清除未凑满一个档位的计数余量，避免切换 LED 时亮度跳变。
                encoder_previous_count = (uint16_t)__HAL_TIM_GET_COUNTER(&htim1); //保存当前计数值，避免切换 LED 时亮度跳变。
                if (selected_led == 0U) {
                    OLED_ShowString(1, 1, "LED1 Duty:");
                } else {
                    OLED_ShowString(1, 1, "LED2 Duty:");
                }
                OLED_ShowNum(2, 1, led_duty_percent[selected_led], 3);
                OLED_ShowChar(2, 4, '%');
            }
        }

        /* 获取 TIM1 当前计数，并计算本次轮询期间的计数变化量。 */

        uint16_t encoder_count = (uint16_t)__HAL_TIM_GET_COUNTER(&htim1); // 获取 TIM1 当前计数值

        // 计算本次轮询期间的计数变化量
        int32_t encoder_delta = (int32_t)encoder_count - (int32_t)encoder_previous_count;
        encoder_previous_count = encoder_count; // 保存本次计数值，供下次计算差值使用。

        // 修正 16 位计数器向上或向下回绕造成的差值跳变。
        // 如当前为65532，上次为 0，则差值为 65532 - 0 = 65532，实际应为 -4,所以需要减去 65536；
        // 如当前为 0，上次为 65532，则差值为 0 - 65532 = -65532，实际应为 +4，所以需要加上 65536。
        if (encoder_delta > 32767) {
            encoder_delta -= 65536;
        } else if (encoder_delta < -32768) {
            encoder_delta += 65536;
        }

        /* 编码器有变化时刷新 OLED 第三行，避免额外保存一份显示计数。 */
        if (encoder_delta != 0) {
            OLED_ShowNum(3, 10, encoder_count, 5);
        }

        /* 累加计数变化，，每满一个编码器档位就将亮度调整 1%。 */
        encoder_count_remainder += encoder_delta;
        int32_t duty_steps = encoder_count_remainder / ENCODER_COUNTS_PER_STEP; // 计算亮度变化步数，整除4，也就是每转过一个档位就增加或减少 1%。
        encoder_count_remainder %= ENCODER_COUNTS_PER_STEP; // 保留未凑满一个档位的计数余量，供下次累加使用。对于负数取余
        if (duty_steps != 0) {
            int32_t new_duty = (int32_t)led_duty_percent[selected_led] + duty_steps;
             // 限制亮度范围在 0~100% 之间
            if (new_duty < 0) new_duty = 0;
            if (new_duty > PWM_DUTY_MAX) new_duty = PWM_DUTY_MAX;

            // 只有当亮度变化时才更新 PWM 比较值和 OLED 显示，避免重复刷新。
            if (new_duty != led_duty_percent[selected_led]) {
                led_duty_percent[selected_led] = (uint8_t)new_duty;
                /* 主板 LED 低电平点亮，亮度越高需缩短 PWM 高电平时间。 */
                uint32_t pulse = PWM_DUTY_MAX - led_duty_percent[selected_led];
                // 根据当前选中的 LED，选择对应的 TIM2 通道，并更新比较值。
                uint32_t channel = (selected_led == 0U) ? TIM_CHANNEL_3 : TIM_CHANNEL_4;
                __HAL_TIM_SET_COMPARE(&htim2, channel, pulse);

                /* 亮度变化后刷新 OLED 数值。 */
                OLED_ShowNum(2, 1, led_duty_percent[selected_led], 3);
                OLED_ShowChar(2, 4, '%');
            }
        }

        /* 短暂延时，限制主循环轮询频率。 */
        HAL_Delay(2);
        /* USER CODE END 3 */
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
