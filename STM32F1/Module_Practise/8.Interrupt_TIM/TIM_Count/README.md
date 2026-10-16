# interrupt_TIM_Count

这是一个基于 STM32F103xB 的定时器中断计数实验工程，主要用于验证 TIM1 的计数、溢出和中断触发逻辑。

## 项目结构

```text
project-tree
interrupt_TIM_Count/
├── CMakeLists.txt                # CMake 构建入口
├── CMakePresets.json             # 编译预设配置
├── startup_stm32f103xb.s         # STM32 启动文件
├── STM32F103xx_FLASH.ld          # Flash/RAM 链接脚本
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── gpio.h
│   │   ├── tim.h
│   │   ├── OLED.h
│   │   ├── stm32f1xx_hal_conf.h
│   │   └── stm32f1xx_it.h
│   └── Src/
│       ├── main.c               # 主程序
│       ├── gpio.c               # GPIO 初始化
│       ├── tim.c                # TIM1 定时计数与中断
│       ├── OLED.c               # OLED 显示驱动
│       ├── stm32f1xx_it.c       # 中断处理
│       └── system_stm32f1xx.c  # 系统时钟初始化
├── Drivers/                     # STM32 HAL / CMSIS 驱动
├── cmake/                       # CMake 工具链和 CubeMX 配置
├── build/                       # 编译输出目录
├── interrupt_TIM_Count.ioc      # STM32CubeMX 配置文件
└── README.md                    # 项目说明
```

该工程主要包含：
- `Core`：核心应用逻辑与外设初始化
- `Drivers`：STM32 官方 HAL 驱动库
- `cmake`：编译工具链配置
- `build`：生成的编译产物

## 当前实现功能

### 1. TIM1 基本计数器配置
- 使用 TIM1 作为普通向上计数定时器
- 配置为 `TIM_COUNTERMODE_UP`
- `Period = 50000`
- 计数器从 `0` 开始计数，计数到 `50000` 时触发一次更新/溢出事件
- 实际代码中每次溢出后会执行一次 `HAL_TIM_PeriodElapsedCallback()`

### 2. 计数值获取
工程中已经封装了以下函数：

- `TIM_GetCurrentCounterValue()`
  - 获取当前 TIM 计数器值

- `TIM_GetOverflowCount()`
  - 获取当前溢出次数 `overValue`

- `TIM_GetTotalCountValue()`
  - 计算总计数值：
    - `溢出次数 * (Period + 1) + 当前计数值`
  - 这里的 `Period + 1` 对应实际代码中 `htim1.Init.Period + 1U`，因为计数范围为 `0 ~ Period`

### 3. 中断处理逻辑
- 在 `HAL_TIM_PeriodElapsedCallback` 中处理 TIM 溢出事件
- 每次溢出时 `overValue++`
- 当 `overValue >= 1000` 时，清零 `overValue` 并调用 `GPIO_Toggle_Status()` 翻转 LED
- 这样可以避免溢出计数器无限增长，并让 LED 以较稳定的节奏翻转

### 4. GPIO 控制
- 提供了 `GPIO_Toggle_Status()`
- 用于控制 `LED_Green_Pin` 的状态翻转

### 5. OLED 显示
- 通过 `OLED_Init()` 和 `OLED_ShowNum()` 等函数显示：
  - 当前计数值
  - 溢出次数
  - 总计数值

## 说明

这个工程适合用于学习：

- TIM 的计数模式
- 溢出中断
- 计数值累计与读取
- GPIO 开发和 OLED 显示联动

如果需要进一步扩展，可以继续加入：

- 更精确的计数周期控制
- 串口输出计数值
- LED 或 OLED 的更复杂状态显示
