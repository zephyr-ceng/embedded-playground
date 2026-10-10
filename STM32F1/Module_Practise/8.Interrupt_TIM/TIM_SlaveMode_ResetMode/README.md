# TIM 从模式复位模式示例

本项目基于 STM32F103 和 STM32 HAL，演示 TIM1 从模式的复位模式（Reset Mode）及更新中断计数，并通过 OLED 显示定时器当前计数值和更新事件累计次数。

## 工作方式

- TIM1 使用内部时钟向上计数，预分频值为 7199，自动重装载值为 50000。
- PA8（TIM1_CH1）配置为上升沿触发输入；输入触发时，复位模式会重新初始化计数器。
- TIM1 更新中断中累计更新事件次数。
- OLED 第一行显示计数值（`CV`），第二行显示更新计数（`OC`）。

## 硬件连接

- MCU：STM32F103 系列（工程启动文件及链接脚本针对 STM32F103xB）。
- 触发信号：接入 PA8，并与开发板共地。
- OLED：软件 I²C，SCL 接 PB11，SDA 接 PB12；按模块要求连接电源和地。

## 构建

使用 CMake Presets 构建。需要 CMake、Ninja 和 ARM GNU Toolchain（`arm-none-eabi-gcc`）；在 VS Code 中可选择 `Debug` 或 `Release` 配置及对应构建预设。工程配置文件为 `TIM_SlaveMode_ResetMode.ioc`。