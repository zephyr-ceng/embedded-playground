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
TIM_HandleTypeDef htim1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static volatile uint32_t Timer_Overflow_Count = 0;

// 获取溢出计数值的函数
static uint32_t Get_Overflow_Value(void)
{
    return Timer_Overflow_Count;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */    int main(void)
    {
      /* USER CODE BEGIN 1 */
      /* USER CODE END 1 */
    
      /* MCU Configuration--------------------------------------------------------*/
      HAL_Init();
      SystemClock_Config();
    
      /* Initialize all configured peripherals */
      MX_GPIO_Init();
      MX_TIM1_Init();
    
      /* USER CODE BEGIN 2 */
      /* USER CODE END 2 */
    
      while (1)
      {
        /* USER CODE END WHILE */
    
        /* USER CODE BEGIN 3 */
        /* USER CODE END 3 */
      }
    }

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

    MX_GPIO_Init();                   // 初始化GPIO
    MX_TIM1_Init();                   // 初始化TIM1作为外部脉冲计数器
    __HAL_TIM_SET_COUNTER(&htim1, 0); // 将计数器清零
    Timer_Overflow_Count = 0;         // 初始化溢出计数器

    /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start_IT(&htim1); // 启动TIM1的中断功能
    OLED_Init();
    OLED_ShowString(1, 1, "Overflow:");
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        uint32_t Overflow_Count = Get_Overflow_Value();
        uint32_t Current_Count = __HAL_TIM_GET_COUNTER(&htim1);
        uint32_t Total_Pulses = Overflow_Count * 10u + Current_Count;

        /* USER CODE END WHILE */
        OLED_ShowNum(2, 1, Current_Count, 6);
        OLED_ShowNum(3, 1, Overflow_Count, 6);
        OLED_ShowString(4, 1, "TCNT:");
        OLED_ShowNum(4, 6, Total_Pulses, 6);
        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{

    /* USER CODE BEGIN TIM1_Init 0 */

    /* USER CODE END TIM1_Init 0 */

    TIM_SlaveConfigTypeDef sSlaveConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM1_Init 1 */

    /* USER CODE END TIM1_Init 1 */
    /* TIM1 用作外部脉冲计数器 */
    /* 计数器工作在向上计数模式，周期设为 9，表示每 10 个脉冲触发一次溢出 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;                                      // 不使用预分频，直接计数外部脉冲
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;                   // 向上计数模式
    htim1.Init.Period = 9;                                         // 设置计数器周期为 9，表示每 10 个脉冲触发一次溢出
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;             // 不使用时钟分频
    htim1.Init.RepetitionCounter = 0;                              // 不使用重复计数器
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE; // 禁用自动重载预装载

    /* 使能 TIM1 时钟 */
    __HAL_RCC_TIM1_CLK_ENABLE();
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }

    /* TIM1 外部脉冲从 TI1 输入触发计数 */
    /* TIM_TS_TI1FP1 表示使用 TI1 的滤波后脉冲信号作为触发源 */
    /* 这里配置为检测上升沿，意味着脉冲的上升沿触发计数一次 */
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1; // 外部触发模式 1，使用外部脉冲作为计数源
    sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
    sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
    sSlaveConfig.TriggerFilter = 0;
    if (HAL_TIM_SlaveConfigSynchro(&htim1, &sSlaveConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /* 其他主控配置保持默认，不使用主输出触发 */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM1_Init 2 */

    /* USER CODE END TIM1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    /* USER CODE BEGIN MX_GPIO_Init_1 */

    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE(); /* GPIO 端口时钟使能：PA8 用作 TIM1 的 TI1 外部脉冲输入 */

    /* PA8 配置为输入模式，等待外部脉冲信号 */
    /* 这里不使用上拉/下拉，外部信号源提供电平变化 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */

    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// TIM1 溢出中断回调函数，每当计数器溢出时调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim1)
    {
        Timer_Overflow_Count++;
    }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
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
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
