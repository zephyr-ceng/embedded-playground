# Interrupt_TIM_Slavemode

这是一个基于 STM32F103 的定时器从模式（Slave Mode）控制器示例项目，重点演示 TIM1 在外部触发条件下按从模式工作，并通过中断与 OLED 显示计数状态。

## 项目用途

该工程主要用于验证和学习以下内容：

- TIM1 作为定时器从模式控制器工作
- 外部触发信号驱动定时器计数/重置
- 基于 `TIM_TS_TI1FP1` 的外部触发配置
- 中断方式处理定时器溢出计数
- 通过 OLED 实时显示当前计数和累计计数

在这个项目中，TIM1 被配置为从模式，输入触发源来自 TIM1_CH1（PA8），并通过外部上升沿触发计数同步动作。计数器溢出时触发更新中断，程序会统计溢出次数，并结合当前计数值计算总计数值。

## 关键功能说明

### 1. TIM1 从模式配置

关键配置位于：

- `Core/Src/tim.c`
- `MX_TIM1_Init()`

配置要点：

- `htim1.Instance = TIM1`
- `htim1.Init.Period = 10`
- `sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1`
- `sSlaveConfig.InputTrigger = TIM_TS_TI1FP1`
- `sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING`

这表示 TIM1 在外部触发沿到来时，进入从模式同步计数工作状态。

### 2. 定时器中断与溢出统计

在：

- `Core/Src/tim.c`

实现了：

- `HAL_TIM_PeriodElapsedCallback()`
- `return_counter()`
- `return_total_counter()`

功能为：

- 计数器每次发生更新/溢出时，增加 `Timer_Overflow_Count`
- 把计数器重置为 0
- 返回当前溢出次数与累计计数值

### 3. 主程序显示

在：

- `Core/Src/main.c`

主要逻辑：

- 初始化 GPIO、TIM1
- 启动 TIM1 中断
- 初始化 OLED
- 在循环中读取：
  - 当前计数 `__HAL_TIM_GET_COUNTER(&htim1)`
  - 溢出计数 `return_counter()`
  - 总计数 `return_total_counter()`
- 通过 OLED 实时显示

## 文件结构说明

```text
Interrupt_TIM_Slavemode/
├── CMakeLists.txt                # CMake 工程入口
├── CMakePresets.json             # CMake 预设配置
├── README.md                     # 项目说明
├── Interrupt_TIM_Slavemode.ioc   # STM32CubeMX 配置文件
├── startup_stm32f103xb.s         # 启动文件
├── STM32F103xx_FLASH.ld          # 链接脚本
├── build/                        # 编译输出目录
├── cmake/                        # 工具链/STM32CubeMX 相关配置
├── Core/
│   ├── Inc/
│   │   ├── gpio.h
│   │   ├── main.h
│   │   ├── OLED.h
│   │   ├── OLED_Font.h
│   │   ├── stm32f1xx_hal_conf.h
│   │   ├── stm32f1xx_it.h
│   │   └── tim.h
│   └── Src/
│       ├── gpio.c
│       ├── main.c
│       ├── OLED.c
│       ├── stm32f1xx_hal_msp.c
│       ├── stm32f1xx_it.c
│       ├── syscalls.c
│       ├── sysmem.c
│       ├── system_stm32f1xx.c
│       └── tim.c
├── Drivers/
│   ├── CMSIS/
│   └── STM32F1xx_HAL_Driver/
└── ...
```

## 典型应用场景

该项目适用于：

- 学习 STM32 定时器从模式
- 外部事件计数与同步控制
- 事件触发型计数器设计
- 需要使用 TIM1 作为外部触发同步器的嵌入式控制场景

## 编译说明

该工程使用 CMake 构建，适配 STM32 GCC / ARM 工具链。通常可在项目根目录执行：

```bash
cmake -S . -B build
cmake --build build
```

如需使用特定预设配置，可结合 `CMakePresets.json` 配置编译环境。

## 备注

这是一个用于学习和验证定时器从模式控制机制的工程，适合配合 STM32CubeMX、HAL 库和示波器/逻辑分析仪一起理解 TIM1 的输入触发、计数和中断行为。
