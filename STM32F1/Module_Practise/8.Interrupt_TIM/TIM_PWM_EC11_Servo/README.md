# TIM_PWM_EC11_Servo

基于 STM32F103C8T6 的定时器练习项目：使用 TIM1 读取 EC11 增量式编码器，通过 TIM3_CH4 输出 PWM 控制舵机，并用 OLED 显示编码器计数和 PWM 比较值。

## 功能概览

- **TIM1 编码器接口**：读取连接到 PA8（TIM1_CH1）和 PA9（TIM1_CH2）的编码器信号。
- **TIM3 PWM 输出**：TIM3_CH4 从 PB1 输出 PWM，当前配置的周期约为 20 ms（50 Hz）。
- **OLED 显示**：显示编码器计数（`EC`）和 PWM 比较值（`DT`）。OLED 使用软件 I²C，当前驱动定义 SCL 为 PB11、SDA 为 PB12。
- **计数到脉宽映射**：程序将有效编码器计数 0～20 映射到比较值约 500～2500，对应高电平脉宽约 0.5～2.5 ms。

## 硬件连接

| 模块 | STM32F103C8T6 引脚 | 说明 |
| --- | --- | --- |
| EC11 A 相 | PA8 | TIM1_CH1 |
| EC11 B 相 | PA9 | TIM1_CH2 |
| 舵机信号线 | PB1 | TIM3_CH4 PWM 输出 |
| OLED SCL | PB11 | 软件 I²C 时钟 |
| OLED SDA | PB12 | 软件 I²C 数据 |
| 舵机电源地 | STM32 GND | 必须共地 |

舵机应使用符合其规格的独立电源，不要默认由开发板 3.3 V 引脚供电。务必将舵机电源地与 STM32 地连接。接线前确认舵机电源电压和信号电平要求。

## 定时器参数

系统时钟配置为 72 MHz。APB1 总线时钟为 36 MHz；STM32F1 在 APB1 分频不为 1 时，定时器时钟为 APB1 时钟的 2 倍，因此 TIM3 时钟为 72 MHz。

TIM3 配置为预分频值 `72 - 1`、自动重装载值 `20000 - 1`：

- 计数频率：$72\,\text{MHz} / 72 = 1\,\text{MHz}$，每个计数约为 1 μs。
- PWM 周期：$20000 \times 1\,\mu\text{s} = 20\,\text{ms}$，频率约 50 Hz。
- 比较值 `CCR = 500` 时，高电平约 0.5 ms；`CCR = 1500` 时约 1.5 ms；`CCR = 2500` 时约 2.5 ms。

当前映射代码的逻辑是：

```c
#define ENCODER_MAX 20

duty = 500 + encoder_count * 2000 / ENCODER_MAX;
__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, duty);
```

在 `encoder_count` 为 0～20 时，`duty` 约从 500 变化到 2500，每个计数档约变化 100 μs。`duty` 是定时器比较值（近似以微秒为单位），不是百分比。

也可以先将编码器计数映射为占空比百分数，再换算为定时器比较值：

```c
float duty_percent = 2.5f + (float)encoder_count * 10.0f / ENCODER_MAX;
duty = (uint16_t)(duty_percent * 20000.0f / 100.0f);
__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, duty);
```

这里占空比范围为 2.5%～12.5%；乘以 20000（PWM 周期计数）再除以 100，即得到 CCR 值。由于当前每个计数约为 1 μs，这也等价于约 500～2500 μs 的高电平脉宽。百分比写法更直观地表达占空比，但使用浮点运算；前面的整数映射结果相同且更适合此类嵌入式计算。两种写法选一种使用即可。

## 舵机简介与调试建议

常见的 hobby 舵机内部包含直流电机、减速齿轮、位置反馈电位器和控制电路。它接收周期性控制脉冲，并根据脉宽转到相应位置；常见控制周期约 20 ms，脉宽常在约 1～2 ms 范围内变化。具体脉宽与角度的对应关系由舵机型号决定，并非所有舵机都能达到完整 180°。

当前程序输出约 0.5～2.5 ms，超出部分舵机的常规范围，可能导致行程受限、碰到机械限位、异响或过热。建议：

1. 先用保守范围测试，例如 1.0～2.0 ms，即比较值约 1000～2000。
2. 确认运动方向和行程后，再逐步扩大范围；一旦舵机顶住、异响或明显发热，立即减小脉宽范围并断电检查。
3. 不要把“180°”视为所有舵机的保证值；按舵机数据手册校准脉宽端点。
4. 若调整编码器时每一档动作不明显，可减小编码器最大值或提高映射范围内每档的脉宽增量；但必须确保最终脉宽仍在舵机允许范围内。

### 当前代码行为提示

主循环将计数限制在 0～20：计数达到 20 后钳位到 20；计数器读数大于 60000 时复位为 0。该处理方式适用于当前练习逻辑，但不是有符号的位置跟踪；若需要编码器正反转双向控制，应额外设计带符号计数和边界处理。

## 构建

项目使用 CMake、Ninja 和 ARM GNU 工具链（`arm-none-eabi-gcc`），适用于 VS Code 的 CMake Tools 工作流：

1. 安装 CMake、Ninja 和 Arm GNU Toolchain，并确保工具链可用。
2. 在 VS Code 中打开项目目录，使用 CMake Tools 选择配置预设和工具链。
3. 选择 Debug 配置并执行 **CMake: Build**。
4. 成功构建后，目标程序为 `TIM_PWM_EC11_Servo.elf`，生成位置取决于所选的 CMake 构建目录。

## 主要文件

- `Core/Src/main.c`：系统初始化、编码器/PWM 启动、显示和控制循环。
- `Core/Src/tim.c`、`Core/Inc/tim.h`：TIM1 编码器及 TIM3 PWM 配置。
- `Core/Src/OLED.c`、`Core/Inc/OLED.h`、`Core/Inc/OLED_Font.h`：OLED 软件 I²C 驱动和字库。
- `TIM_PWM_EC11_Servo.ioc`：STM32CubeMX 项目配置。
- `CMakeLists.txt`、`cmake/`：CMake 构建配置。
