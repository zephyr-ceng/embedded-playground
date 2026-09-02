# STM32F103 TIM1 ETR 外部时钟计数示例

## 项目功能

本项目演示 STM32F103 的 TIM1 使用 ETR（External Trigger）引脚作为外部时钟源计数，并在 OLED 上显示计数值：

- 使用外接按键连接 `PA12/TIM1_ETR`。
- PA12 使用外部上拉电阻保持高电平，按键按下时将 PA12 接到 GND，TIM1 使用 ETR 下降沿计数。
- 每次按下按键产生一个 ETR 外部脉冲。
- 主循环使用 `Delay_ms(20)` 分别确认按下和释放状态，OLED 只显示消抖后的有效按键次数。
- OLED 初始化后显示 `ETR Count:` 和消抖后的有效按键次数。

## 运行流程

```text
外接按键按下
  -> PA12/TIM1 ETR 下降沿
  -> TIM1 外部时钟计数
  -> main.c 读取 TIM1 计数器
  -> OLED 显示计数值
```

如果使用外部脉冲源，也可以保持以下连接方式：

```text
外部脉冲
  -> TIM1 ETR 输入
  -> TIM1 外部时钟计数
  -> main.c 读取 TIM1 计数器
  -> OLED 显示计数值
```

## 软件架构

```text
src/
├─ main.c                         应用入口和主循环
├─ stm32f10x_it.c/.h              Cortex-M3/外设中断处理模板
└─ drive/
   ├─ inc/                        驱动接口
    │  ├─ Timer.h                  TIM1 ETR 计数接口
   │  ├─ OLED.h                   OLED 显示接口
   │  ├─ MyDelay.h                延时接口
   │  └─ 其他外设头文件
   └─ src/                        驱动实现
      ├─ Timer.c                  TIM1 ETR 外部时钟和计数功能配置
      ├─ OLED.c                   OLED 驱动
      ├─ MyDelay.c                延时实现
      └─ 其他外设驱动

hal/STM32F10x_StdPeriph_Driver/   STM32F1 标准外设库
lib/cmsis/                        CMSIS 内核与编译器接口文件
stm32f1x_64KB_flash.ld            链接脚本
```

## 关键配置

- 外部输入：`GPIOA Pin 12`（TIM1 ETR）。
- 定时器：`TIM1`，无预分频、向上计数。
- 外部时钟：使用 ETR 外部时钟模式 2（`TIM_ETRClockMode2Config`），通过 `SMCR.ECE` 使能 ETR 直接作为计数时钟。
- 输入极性：反相，PA12 的有效下降沿使 `TIM1_CNT` 加 1。
- ETR 滤波：关闭（滤波值为 `0`）；ETR 预分频：关闭（`TIM_ExtTRGPSC_OFF`）。
- 自动重装载值：`65535`（代码配置为 `0xFFFF`）。
- 显示设备：OLED，由 `OLED.c` 提供驱动。

模式 2 的信号路径为：

```text
PA12/TIM1_ETR
  -> ETR 极性检测与滤波
  -> ETR 预分频器
  -> ETR 外部时钟模式 2（SMCR.ECE=1）
  -> TIM1_CNT
```

该模式不通过从模式控制器的 `SMS=外部时钟模式 1` 和 `TS=ETRF` 选择触发源；
如需使用外部时钟模式 1，应改用 `TIM_ETRClockMode1Config`。

## 构建与下载

本工程使用 VS Code 配合 Embedded IDE（EIDE）管理。打开工作区后，可执行：

1. `build`：编译工程。
2. `flash`：下载程序到目标板。
3. `build and flash`：编译并下载。

使用按键时，请将 PA12 通过外部上拉电阻连接到 3.3V，按键另一端连接 GND；按下按键会在 PA12 产生下降沿。使用其他外部脉冲源时，请将信号连接到 PA12，并确保信号电平满足 STM32F103 GPIO 输入要求。
