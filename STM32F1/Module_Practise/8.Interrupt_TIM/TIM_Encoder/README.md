# TIM_Encoder

基于 STM32F103C8T6 的定时器编码器接口与 PWM 练习工程，使用 STM32 HAL、CMake 和 GNU Arm Embedded Toolchain。工程中另加入了通过 GPIO 模拟 I²C 驱动的 OLED 模块。

启动后，TIM1 编码器旋转调节当前选中 LED 的亮度；EC11 按键在 LED1、LED2 之间切换调节目标。TIM2_CH3/CH4 分别输出 LED1/LED2 的 PWM，两路亮度独立保存，OLED 显示选中 LED 的亮度和 TIM1 当前计数。

## 硬件与时钟

- MCU：STM32F103C8T6，LQFP48
- 存储器：Flash 64 KB，SRAM 20 KB
- 系统时钟：外部高速时钟 HSE 经 PLL ×9，SYSCLK/HCLK 为 72 MHz
- APB1：36 MHz；APB1 定时器时钟为 72 MHz
- APB2：72 MHz
- 调试接口：SWD（PA13/SWDIO、PA14/SWCLK）

## 外设配置

| 外设 | 配置 | 引脚 |
| --- | --- | --- |
| TIM1 | 编码器接口模式 TI12；向上计数；自动重装值 65535；输入滤波器为 0 | PA8：TIM1_CH1；PA9：TIM1_CH2 |
| TIM2 | PWM 模式 1，通道 3/4；预分频值 71；自动重装值 ARR=99（`100 - 1`，周期 100 个计数）；启动时两路比较值 50 | PA2：TIM2_CH3（LED1）；PA3：TIM2_CH4（LED2） |
| EC11 按键 | 上拉输入、低电平按下；软件消抖 20 ms | PA15 |
| OLED | GPIO 模拟 I²C；SCL/SDA 开漏输出 | PB11：SCL；PB12：SDA |

### 主程序中的值处理

1. 初始化 TIM1 编码器，设置 LED1/LED2 初始亮度为 50%，分别启动 TIM2_CH3/CH4；OLED 显示当前调节目标和亮度。
2. PA15 按键低电平有效，稳定 20 ms 后确认按下并切换调节目标，同时清除未凑满一步的计数余量，避免切换前的部分计数影响另一颗 LED。PA15 同时是 STM32 的 JTAG 引脚，GPIO 初始化会关闭 JTAG、保留 SWD。
3. TIM1 计数范围为 0～65535，溢出后回到 0。主循环计算“本次值 - 上次值”，并用半周期阈值 32768 修正跨边界造成的跳变。例如 65534→2 的原始差值为 -65532，加 65536 后得到 +4；1→65533 的原始差值为 +65532，减 65536 后得到 -4。该方法要求每次轮询间变化小于半周期；当前轮询间隔为 2 ms。
4. 每累计正向 4 个 TIM1 计数，当前选中 LED 亮度增加 1%；反向 4 个计数则降低 1%，范围限制为 0～100%。未凑满 4 的计数保存在 `encoder_count_remainder` 中。两路亮度集中保存在 `led_duty_percent[2]`，按键只切换数组下标，因此互不覆盖。主循环用整数除法一次算出完整步数，再保留余数。
5. 主板 LED 为低电平点亮，因此 PWM 比较值与界面亮度反向映射：`CCR = 100 - 亮度百分比`。例如亮度 80% 时 CCR=20，PWM 高电平（LED 熄灭）约占 20%，低电平点亮约占 80%；亮度 0% 时 CCR=100，LED 熄灭；亮度 100% 时 CCR=0，LED 常亮。LED1 更新 TIM2_CH3，LED2 更新 TIM2_CH4。当前 TIM2 的 `htim2.Init.Period = 100 - 1`，ARR=99、周期长度为 100 个计数，因此可以直接使用该反向映射；若以后修改周期，应按实际 ARR 重新换算。OLED 第二行显示选中 LED 的亮度，第三行显示 TIM1 原始计数。
6. LED1/LED2 分别接 PA2/PA3 并串联限流电阻；共地。旋钮只改变当前选中的 LED，另一 LED 保持之前保存的亮度。

OLED 第一行显示当前目标（`LED1 Duty:` 或 `LED2 Duty:`），第二行显示占空比，第三行显示 TIM1 当前计数。OLED 驱动位于 `Core/Src/OLED.c`，字体数据位于 `Core/Inc/OLED_Font.h`；I²C 从机写地址设为 `0x78`。请按实际 OLED 模块确认接口、电压和地址。

## 软件与构建

- CMake 3.22 或更高版本
- Ninja
- `arm-none-eabi-gcc` 工具链（需在 `PATH` 中可用）
- STM32CubeF1 HAL（工程内 `Drivers/`）

使用已定义的 CMake Preset 构建：

```sh
cmake --preset Debug
cmake --build --preset Debug
```

Release 构建：

```sh
cmake --preset Release
cmake --build --preset Release
```

构建目录分别为 `build/Debug` 和 `build/Release`。链接脚本为 `STM32F103xx_FLASH.ld`；工具链配置位于 `cmake/gcc-arm-none-eabi.cmake`。

## 目录概览

- `Core/Inc/`、`Core/Src/`：应用头文件和源文件，包括 GPIO、定时器及 OLED 驱动
- `Drivers/`：CMSIS 与 STM32F1 HAL 驱动
- `cmake/`：工具链和 CubeMX 生成的 CMake 配置
- `TIM_Encoder.ioc`：STM32CubeMX 工程配置
- `startup_stm32f103xb.s`：启动文件
- `STM32F103xx_FLASH.ld`：Flash/RAM 链接脚本
- `CMakeLists.txt`、`CMakePresets.json`：CMake 工程与构建预设
