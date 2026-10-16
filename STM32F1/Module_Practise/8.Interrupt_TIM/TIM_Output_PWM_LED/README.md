# TIM Output PWM LED

## 1. 项目简介

这个工程是一个基于 STM32F1 系列 MCU 的 PWM 输出示例，演示如何通过通用定时器 TIM2 生成 PWM 波，并将输出连接到 `PA2` 上，控制 LED 或外部负载的亮度变化。

本项目使用的是 STM32 HAL 库，采用 CMake 构建方式，并通过 CubeMX 生成的初始化代码来完成 GPIO、时钟和定时器配置。主程序中不断修改 TIM2 的比较值 `CCR`，从而实现 LED 亮度逐渐变亮和变暗的效果。

---

## 2. 项目目的

该项目的核心目标有 3 个：

1. 理解 STM32 定时器的基本工作原理。
2. 掌握 PWM 的初始化方法和 API 使用方式。
3. 学会用软件改变占空比，控制输出电压平均值或 LED 亮度。

在工程中，`TIM2_CH3` 通过 `PA2` 输出 PWM 信号，且代码中通过循环改变 `__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse);` 来改变占空比，最终实现呼吸灯效果。

---

## 3. 工程结构说明

```text
TIM_Output_PWM_LED/
├── CMakeLists.txt                # CMake 项目入口
├── CMakePresets.json             # CMake 预设配置
├── startup_stm32f103xb.s         # STM32 启动文件
├── STM32F103xx_FLASH.ld          # 链接脚本
├── TIM_Output_PWM_LED.ioc        # STM32CubeMX 配置文件
├── Core/
│   ├── Inc/
│   │   ├── gpio.h
│   │   ├── main.h
│   │   ├── stm32f1xx_hal_conf.h
│   │   ├── stm32f1xx_it.h
│   │   └── tim.h
│   └── Src/
│       ├── gpio.c
│       ├── main.c               # 主程序入口
│       ├── stm32f1xx_hal_msp.c
│       ├── stm32f1xx_it.c
│       ├── syscalls.c
│       ├── sysmem.c
│       ├── system_stm32f1xx.c
│       └── tim.c                # TIM2 PWM 初始化代码
├── Drivers/
│   ├── CMSIS/
│   └── STM32F1xx_HAL_Driver/
├── cmake/
│   ├── gcc-arm-none-eabi.cmake
│   ├── starm-clang.cmake
│   └── stm32cubemx/
├── build/
│   └── Debug/
└── README.md                     # 本文档
```

### 关键文件说明

- `Core/Src/main.c`
  - 调用 `MX_TIM2_Init()` 初始化定时器。
  - 调用 `HAL_TIM_PWM_Start()` 启动 PWM。
  - 在主循环中逐步修改比较值，实现 PWM 占空比变化。

- `Core/Src/tim.c`
  - 配置 `TIM2` 的时基和 PWM 通道。
  - 设置 `PA2` 为 `TIM2_CH3` 复用推挽输出。
  - 遍历 PWM 参数。

- `Core/Inc/tim.h`
  - 对外声明 `TIM2` 处理句柄和初始化函数。

---

## 4. 运行原理概览

该工程的核心行为可以概括为：

1. MCU 启动后初始化时钟和 GPIO。
2. 初始化 `TIM2` 作为 PWM 定时器。
3. 设置 `TIM2_CH3` 输出到 `PA2`。
4. 通过不断修改 CCR 值改变高电平持续时间。
5. LED 或外部负载看到的是不同的“平均功率”，从而产生亮度变化。

---

## 5. TIM2 与 TIM1 高级定时器的简易对比

在 STM32F1 中，`TIM1` 属于高级定时器，而 `TIM2` 属于通用定时器。两者都可以产生 PWM，但在功能和应用侧重点不同：

| 对比项 | TIM2（通用定时器） | TIM1（高级定时器） |
| --- | --- | --- |
| 定时器类型 | 通用定时器 | 高级定时器 |
| 典型用途 | PWM 输出、定时、计数、基础控制 | PWM 输出、高级控制、互补输出、同步控制 |
| 通道数 | 4 个通道（CH1~CH4） | 4 个主通道 + 3 个互补通道（CH1~CH4 + CH1N~CH3N） |
| 功能复杂度 | 相对简单，适合基础控制 | 更强大，适合电机、逆变、三相桥驱动等 |
| 互补 PWM | 不支持 | 支持，适合半桥/全桥驱动 |
| 锁相/同步 | 基本不涉及 | 支持更高级的同步与触发控制 |
| 典型场景 | LED 调光、基础 PWM、脉冲控制 | 电机驱动、三相控制、功率变换 |
| 本项目中的角色 | 负责生成 PWM 波并驱动 LED | 更适合复杂功率控制，不是本例中的使用对象 |

> 简单理解：`TIM2` 更偏“通用、简单、适合入门”；`TIM1` 更偏“高级、复杂、适合功率控制”。本项目使用的是 `TIM2`，因为它足够完成 PWM 生成和 LED 调光需求。

在 STM32F1 里，`TIM1` 还具备更强的高级特性，例如：互补输出、死区时间、BRK/COM 中断等，这些功能通常用于电机控制或开关电源场景，而不是简单 LED 呼吸灯。

---

## 6. PWM 设置重点说明

### 6.1 TIM2 的初始化参数

在 `Core/Src/tim.c` 中，关键配置如下：

```c
htim2.Instance = TIM2;
htim2.Init.Prescaler = 72 - 1;          // 预分频器
htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
htim2.Init.Period = 100 - 1;            // 自动重装载值
htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
```

对应含义：

- `Prescaler = 71`
  - 表示分频值为 72。
- `Period = 99`
  - 表示计数器从 0 计数到 99，再回到 0 重新开始。
- 所以一个 PWM 周期对应一个计数器周期。

### 6.2 PWM 频率计算

对 STM32F1 来说，TIM2 的计数时钟通常是 APB1 总线时钟按倍频后提供的，实际这里可按 72 MHz 计入定时器输入时钟。计算公式为：

$$
F_{PWM} = \frac{TIMER\_CLK}{(Prescaler + 1) \times (Period + 1)}
$$

代入本工程参数：

$$
F_{PWM} = \frac{72\,MHz}{72 \times 100} = 10\,kHz
$$

也就是：

- 计数时钟 = 1 MHz
- 周期值 = 100
- PWM 频率 = 10 kHz

这样输出波形的周期约为：

$$
T = \frac{1}{10\,kHz} = 100\,\mu s
$$

### 6.3 PWM 通道配置

```c
sConfigOC.OCMode = TIM_OCMODE_PWM1;
sConfigOC.Pulse = 50;
sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3);
```

说明：

- `TIM_OCMODE_PWM1`：使用 PWM 模式 1
- `Pulse`：设置比较值 `CCR`
- `TIM_CHANNEL_3`：使用 TIM2 的第 3 通道
- `PA2` 对应 `TIM2_CH3`

此外，主函数中启动 PWM：

```c
HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
```

---

## 7. 占空比原理

占空比（Duty Cycle）是 PWM 中最核心的概念，它表示一个周期内，高电平持续时间所占比例。

### 7.1 公式

$$
DutyCycle = \frac{T_{HIGH}}{T_{period}} \times 100\% = \frac{CCR}{ARR + 1} \times 100\%
$$

其中：

- `CCR`：捕获/比较寄存器值，也就是当前脉冲宽度寄存器
- `ARR`：自动重装载寄存器，决定 PWM 周期的上限

在本工程中：

- `ARR = 99`
- `CCR = pulse`

因此：

$$
DutyCycle = \frac{pulse}{100} \times 100\%
$$

举例：

- `pulse = 25`，占空比 = 25%
- `pulse = 50`，占空比 = 50%
- `pulse = 75`，占空比 = 75%

### 7.2 代码中的实际应用

主程序中是通过下面这行代码来改变 PWM 的占空比：

```c
__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse);
```

其中 `pulse` 一直在 0 ~ 100 之间循环变化，所以输出占空比也在 0% ~ 100% 之间逐步改变，形成亮度从暗到亮再回到暗的效果。

### 7.3 高频、低占空比和高占空比的直观理解

- 占空比小：高电平时间短，平均电压低，输出看起来更暗。
- 占空比大：高电平时间长，平均电压更高，输出看起来更亮。
- 频率不变，只改变占空比，可以改变“能量大小”。

例如 LED 亮度控制中：

- 10% 占空比：几乎暗
- 50% 占空比：中等亮度
- 90% 占空比：接近全亮

### 7.4 PWM 模式 2 扩展说明

STM32 的 PWM 具有两种常见模式：`PWM1` 和 `PWM2`。在本工程中使用的是 `TIM_OCMODE_PWM1`，这表示：

- 在向上计数时，`CNT < CCR` 时输出有效电平；
- 当 `CNT >= CCR` 时输出无效电平；
- 因此输出信号高电平时间等于 `CCR` 对应的脉冲宽度。

而如果改成 `TIM_OCMODE_PWM2`，其逻辑正好相反：

- 在向上计数时，`CNT < CCR` 时输出无效电平；
- 当 `CNT >= CCR` 时输出有效电平；
- 也就是说，电平极性与模式 1 恰好相反。

可以用一句话理解：

- `PWM1`：有效电平在高电平区段中“先高后低”
- `PWM2`：有效电平在低电平区段中“先低后高”

其本质都是通过比较 `CNT` 和 `CCR` 来控制输出占空比，只是输出的有效状态相反。

例如：

- 若 `ARR = 99`，`CCR = 25`
- 在 `PWM1` 中，高电平持续时间为 25/100 周期，低电平持续 75/100 周期
- 在 `PWM2` 中，高电平持续时间为 75/100 周期，低电平持续 25/100 周期

也就是说，`PWM2` 会在相同的周期和相同 `CCR` 条件下，生成一个“相反极性的 PWM 输出”。

这时候，若我们从“占空比”的角度理解，则两者虽然逻辑相反，但都可以达到同样的平均功率效果，只是输出波形的相位关系不同。

> 对于 LED 调光而言，`PWM1` 和 `PWM2` 都可以实现亮度控制；如果需要“高电平有效”按常见习惯来驱动 LED，则通常使用 `PWM1`；如果需要“低电平有效”或反转输出，则使用 `PWM2`。

---

## 8. 主程序逻辑说明

在 `main.c` 中，程序流程非常简单：

```c
MX_GPIO_Init();
MX_TIM2_Init();
HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
```

随后进入死循环：

```c
for (uint16_t pulse = 0; pulse <= 100; pulse++) {
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse);
    HAL_Delay(10);
}

for (uint16_t pulse = 100; pulse > 0; pulse--) {
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse);
    HAL_Delay(10);
}
```

这段代码实现了“呼吸灯”效果：

- 占空比从 0% 增大到 100%
- 再从 100% 下降到 0%
- 反复循环

这说明 PWM 输出可以用于电机调速、LED 调光、发光强度控制等多种应用。

---

## 9. 构建和编译

本项目用 CMake 方式构建，推荐使用如下命令：

```bash
cmake --preset Debug
cmake --build --preset Debug
```

如果使用的是已生成的 build 目录，也可以直接在该目录执行编译：

```bash
cmake --build build/Debug
```

---

## 10. 总结

本工程是 STM32 定时器 PWM 的典型入门示例，重点体现了以下内容：

- `TIM2` 作为通用定时器生成 PWM
- `PA2` 上的 `TIM2_CH3` 输出 PWM
- 通过调节 `CCR` 数值来改变占空比
- 使用占空比控制 LED 亮度，形成呼吸灯效果

如果理解了这一个工程，后续再学习电机控制、舵机控制、照明控制、ADC 采样同步等会更容易上手。

---

## 11. 备注

该工程适合用于：

- 初学者学习 STM32 定时器
- 理解 PWM 工作原理
- 进行 LED 调光实验
- 进一步扩展到电机和伺服控制

如果需要，我也可以继续补充一份“PWM 公式推导版说明”或者“按时序流程图解释 TIM2 PWM 生成过程”的扩展文档。
