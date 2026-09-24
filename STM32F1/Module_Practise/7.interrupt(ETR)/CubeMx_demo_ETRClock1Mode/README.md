# CubeMx_demo_ETRClock1Mode

这是一个基于 STM32F1 HAL 的学习工程，当前重点演示 TIM1 在外部脉冲计数场景下的触发配置、溢出计数与 OLED 显示。该项目用于研究 ETR / TI1FP1 触发模式、外部从模式计数以及中断处理流程。

## 项目概览

- MCU: STM32F103xB
- Core: Cortex-M3
- Compiler: arm-none-eabi-gcc
- HAL: STM32F1xx HAL Driver
- IDE / Workspace: VS Code + EIDE + STM32CubeMX
- 目标用途: 学习 TIM1 外部脉冲计数、更新时间计数、溢出累计，以及 HAL 工程结构

## 当前项目状态

当前工程已经从纯 SPL/旧版驱动写法迁移为 HAL 架构，OLED 驱动保持独立模块化，不依赖生成器内部代码。核心逻辑已经设置为：

- TIM1 作为外部脉冲计数器
- 外部脉冲接入 PA8，对应 TIM1 的 TI1 输入
- `TIM_TS_TI1FP1` 用作触发源
- 触发极性设置为上升沿：`TIM_TRIGGERPOLARITY_RISING`
- 每当计数达到周期值后触发溢出中断，累加 `Timer_Overflow_Count`
- OLED 上显示：当前计数、溢出次数和总脉冲数

该工程已通过当前 HAL 编译验证，编译时使用的配置包括：

- `USE_HAL_DRIVER`
- `STM32F103xB`
- `HAL/Core/Inc`
- `HAL/Drivers/STM32F1xx_HAL_Driver/Inc`
- `HAL/Drivers/CMSIS/Device/ST/STM32F1xx/Include`

## 目录说明

```text
CubeMx_demo_ETRClock1Mode/
├── .eide/                         # EIDE 项目配置目录
├── .vscode/                       # VS Code 工作区配置
├── .clang-format                  # 代码格式化配置
├── build/                         # 编译输出目录
├── HAL/                           # HAL 工程主目录
│   ├── Core/
│   │   ├── Inc/
│   │   │   ├── main.h             # HAL 公共头文件
│   │   │   ├── OLED.h             # OLED 驱动 API
│   │   │   ├── OLED_Font.h        # OLED 字库
│   │   │   └── stm32f1xx_it.h     # 中断头文件
│   │   └── Src/
│   │       ├── main.c             # 用户主程序、TIM1 配置、计数逻辑
│   │       ├── OLED.c             # OLED 独立驱动实现（HAL版）
│   │       ├── stm32f1xx_it.c     # 中断入口
│   │       ├── stm32f1xx_hal_msp.c
│   │       ├── system_stm32f1xx.c
│   │       └── ...
│   ├── Drivers/                   # HAL 驱动和 CMSIS
│   ├── startup_stm32f103xb.s      # 启动文件
│   ├── STM32F103xx_FLASH.ld       # 链接脚本
│   ├── Makefile                   # GNU Make 编译脚本
│   └── cube_ETR_Clock1_demo_TI1FP1.ioc  # CubeMX 配置文件
├── CubeMx_demo_ETRClock1Mode.code-workspace
├── README.md
├── .gitignore
└── ...
```

## 当前关键配置说明

工程的核心逻辑主要位于 [HAL/Core/Src/main.c](HAL/Core/Src/main.c)。当前配置的关键点如下：

- `MX_GPIO_Init()`
  - 初始化 PA8 作为 `TIM1` 的外部脉冲输入端口
  - 配置为输入模式，等待外部信号

- `MX_TIM1_Init()`
  - 配置 TIM1 为上升计数定时器
  - `TIM_SLAVEMODE_EXTERNAL1` 表示使用外部信号作为从模式触发源
  - `TIM_TS_TI1FP1` 表示使用 TI1 的滤波后脉冲信号作为计数触发来源
  - `TIM_TRIGGERPOLARITY_RISING` 表示检测上升沿
  - `Period = 9` 表示计数器在 10 个脉冲后发生一次溢出
  - 需要清理掉默认的中断标志位`HAL_TIM_CLEAR_FLAG(&htim1, TIM_FLAG_UPDATE);`

- `HAL_TIM_PeriodElapsedCallback()`
  - 在 TIM1 更新/溢出中断中执行
  - `Timer_Overflow_Count++`

- `Get_Overflow_Value()`
  - 返回累计溢出次数

- OLED 显示逻辑
  - `OLED_ShowNum(2,1,Current_Count,6);`
  - `OLED_ShowNum(3,1,Overflow_Count,6);`
  - `OLED_ShowString(4,1,"TCNT:");`
  - `OLED_ShowNum(4,6,(Overflow_Count * 10 + Current_Count),6);`

## 默认 CubeMX 生成的 `main.c` 文件结构说明

在 CubeMX 自动生成的 STM32 HAL 工程中，`main.c` 不是只有 `main()` 函数，而是一个完整的“工程入口文件”，它通常由以下几部分组成：

1. 文件头部：包含头文件和必要的宏定义
2. 全局变量与外部声明
3. `main()` 主函数：系统初始化与主循环入口
4. 系统配置函数：如 `SystemClock_Config()`、`MX_GPIO_Init()`、`MX_TIM1_Init()`
5. 中断回调函数：如 `HAL_TIM_PeriodElapsedCallback()`
6. 用户代码保留区：`USER CODE BEGIN / END` 标记
7. 其他辅助函数与扩展逻辑

整体结构大致如下：

```c
#include "main.h"

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

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

void SystemClock_Config(void)
{
  /* ... */
}

static void MX_GPIO_Init(void)
{
  /* ... */
}

static void MX_TIM1_Init(void)
{
  /* ... */
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* ... */
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */
```

这说明 `main.c` 的重点并不只在 `main()`，而是整个“初始化 + 外设配置 + 回调 + 主循环 + 自定义逻辑”的组合文件。

### 常见的区域含义

- `USER CODE BEGIN PTD / END PTD`
  - 用于放置自定义类型定义，如结构体、枚举、类型别名等
  - 常见于：状态机类型、通信帧结构体、配置结构体

- `USER CODE BEGIN PD / END PD`
  - 用于放置自定义宏定义或常量定义
  - 常见于：设备地址、标志位、采样周期、显示参数等

- `USER CODE BEGIN PM / END PM`
  - 用于放置自定义宏函数或复用的小型函数式宏
  - 例如：位掩码、状态判断、组合逻辑的便捷宏

- `USER CODE BEGIN PV / END PV`
  - 用于声明全局变量、状态变量、缓存变量等
  - 适合放置：计数器变量、标志位、缓冲区、设备状态

- `USER CODE BEGIN PFP / END PFP`
  - 用于声明自定义函数原型
  - 让后续函数实现可以在文件后面定义，而不必全部放在前面

- `USER CODE BEGIN 0 / END 0`
  - 这是 CubeMX 中常见的“自定义代码插入区”，通常放置：
    - 静态辅助函数
    - 私有工具函数
    - 全局状态变量
    - 计数器/标志位初始化逻辑
  - 在本工程中，它用于放置 `Timer_Overflow_Count` 和 `Get_Overflow_Value()` 这类私有逻辑

- `USER CODE BEGIN 1 / END 1`
  - 位于 `main()` 开头，适合放置临时变量或初始化前准备逻辑

- `USER CODE BEGIN 2 / END 2`
  - 适合放置外设启动、计数器使能、中断开启、OLED 初始化等初始化代码

- `while(1)` 中的 `USER CODE BEGIN 3 / END 3`
  - 用于放主循环中的检测、刷新、显示更新、状态处理逻辑

- `USER CODE BEGIN 4 / END 4`
  - 通常放置自定义函数、回调函数、辅助函数或扩展功能

### `USER CODE BEGIN 0` 的特殊作用

`USER CODE BEGIN 0` 通常不是主函数入口，而是一个“文件级私有代码区”，更偏向于：

- 私有变量定义
- 私有工具函数
- 默认初始化逻辑
- 不需要对外暴露的辅助代码

它和 `USER CODE BEGIN PV` 的区别在于：

- `PV` 更像“全局变量区”
- `0` 更偏向“文件内私有逻辑区”，通常放更细粒度的辅助代码

这也是 CubeMX 默认生成模板中常见的一种组织方式。

### 这类区域的意义

- 它们是 CubeMX 自动生成代码中的“用户编辑保护区”
- 用户可以在不破坏生成代码结构的前提下，继续扩展功能
- 当再次通过 CubeMX 生成代码时，这些区域通常可以保留用户逻辑
- 这是 STM32CubeMX 官方工程模板中最经典的代码组织方式

### 对当前项目的理解

在本工程中，真正的业务逻辑并不是集中在 `main()` 的一小段代码里，而是分散在：

- `MX_GPIO_Init()`：GPIO 初始化，配置外部脉冲输入
- `MX_TIM1_Init()`：TIM1 触发计数配置
- `HAL_TIM_PeriodElapsedCallback()`：溢出计数处理
- `while(1)`：读取计数值并更新 OLED 显示

也就是说，当前这个 `main.c` 更像是一个“完成初始化、配置外设、响应中断、更新显示”的完整文件结构，而不是单纯的一个 `main()` 函数。
## OLED 驱动说明

当前工程中 OLED 驱动已经调整为独立 HAL 版，保留了原先的驱动 API 接口不变：

- `OLED_Init()`
- `OLED_Clear()`
- `OLED_ShowChar()`
- `OLED_ShowString()`
- `OLED_ShowNum()`
- `OLED_ShowSignedNum()`
- `OLED_ShowHexNum()`
- `OLED_ShowBinNum()`

驱动文件为：

- [HAL/Core/Inc/OLED.h](HAL/Core/Inc/OLED.h)
- [HAL/Core/Src/OLED.c](HAL/Core/Src/OLED.c)
- [HAL/Core/Inc/OLED_Font.h](HAL/Core/Inc/OLED_Font.h)

这样可以保证 OLED 作为独立模块，和 CubeMX 自动生成代码分离，后续无需修改对外函数接口即可继续使用。

## 编译与调试

### 方式 1：VS Code 任务

项目已经配置了以下任务：

- `build`
- `rebuild`
- `clean`
- `build and flash`

### 方式 2：命令行编译

在工程根目录下执行：

```bash
arm-none-eabi-gcc -c ...
```

或使用 EIDE 的项目编译入口。

### 方式 3：烧录和调试

当前工程适合使用 OpenOCD、CMSIS-DAP 或 ST-Link 进行烧录与调试。

## 学习目标

- TIM1 外部触发计数配置
- `TI1FP1` 触发模式与上升沿检测
- HAL 中断回调使用方法
- 独立外设驱动封装方式
- STM32CubeMX + HAL 工程结构理解

## 备注

该工程目前已具备：

- 外部脉冲计数功能
- 溢出累计计数功能
- OLED 动态显示功能
- HAL 版本驱动结构

因此它既可以作为定时器外部触发实验，也可以作为 HAL 工程结构学习示例。