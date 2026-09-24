# GPIO 端口使用之 LED（推挽输出）

本实验基于 STM32F103，使用标准外设库控制 GPIOB 的 PB10 驱动外部 LED。程序采用推挽输出，LED 低电平点亮、高电平熄灭，并以 100 ms 熄灭、900 ms 点亮的周期循环闪烁。

## 项目结构

| 路径 | 说明 |
| --- | --- |
| `src/main.c` | GPIOB 时钟、PB10 初始化和 LED 闪烁主循环 |
| `src/drive/MyDelay.c` | 基于 Cortex-M3 SysTick 的微秒、毫秒和秒延时 |
| `src/drive/MyDelay.h` | 延时函数声明 |
| `src/startup_stm32f10x_md.s` | 中密度 STM32F10x 的启动文件和中断向量表 |
| `hal/STM32F10x_StdPeriph_Driver/` | STM32F10x 标准外设库驱动 |
| `lib/cmsis/` | CMSIS 内核头文件 |
| `stm32f1x_64KB_flash.ld` | 链接脚本，定义程序的 Flash 和 RAM 布局 |
| `build/` | EIDE 生成的编译产物，不作为源代码修改入口 |

### 引脚说明

| 功能 | GPIO 端口 | 引脚 | 有效电平 |
| --- | --- | --- | --- |
| LED | GPIOB | PB10 | 低电平点亮，高电平熄灭 |

### 接线说明

本实验使用面包板连接外部 LED，接线如下：

1. 将开发板的 `3.3V` 和 `GND` 分别连接到面包板的正、负电源轨。
2. 将 LED 插在面包板中间沟槽两侧，避免 LED 两个引脚插在同一组导电孔中。
3. LED 阳极（长脚、正极）通过 `220Ω~1kΩ` 限流电阻连接到 `3.3V` 电源轨。
4. LED 阴极（短脚、负极）连接到开发板的 `PB10` 引脚。

```text
面包板正电源轨（3.3V） ── 限流电阻 ── LED 阳极（长脚）
									  LED 阴极（短脚） ── PB10
面包板负电源轨（GND） ───────────────────────────────（共地）
```

- 该接法由 GPIO 输出低电平时点亮 LED，输出高电平时熄灭 LED。
- 禁止省略限流电阻，或将 LED 直接接到电源和 GPIO 引脚之间。

## 时钟调用逻辑

STM32 的外设复位后通常不会立即获得工作时钟。GPIOB 挂载在 APB2 总线上，因此在访问 GPIOB 的寄存器或调用 `GPIO_Init` 之前，必须先开启 GPIOB 的 APB2 外设时钟：

```c
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
```

这行代码可以拆分为三个部分：

1. `RCC_APB2PeriphClockCmd` 是标准外设库提供的时钟控制函数，用于修改 RCC 外设时钟使能寄存器。
2. `RCC_APB2Periph_GPIOB` 指定 APB2 总线上的 GPIOB 外设。它不是具体引脚编号，PB10 仍由后面的 `GPIO_Pin_10` 指定。
3. `ENABLE` 表示打开该外设时钟；若传入 `DISABLE`，则会关闭 GPIOB 时钟。

本项目的初始化顺序如下：

```text
RCC_APB2PeriphClockCmd(GPIOB, ENABLE)
	↓
GPIO_Init(GPIOB, GPIO_Pin_10, GPIO_Mode_Out_PP, GPIO_Speed_50MHz)
	↓
GPIO_SetBits / GPIO_ResetBits
	↓
Delay_ms 控制闪烁周期
```

如果遗漏时钟使能，GPIOB 外设寄存器无法正常工作，PB10 初始化和输出控制也不能按预期执行。若以后改用 GPIOA、GPIOC 等端口，应将时钟宏替换为对应的 `RCC_APB2Periph_GPIOx`，并保持“先开时钟、后初始化 GPIO”的顺序。

## GPIO 初始化与运行逻辑

`GPIO_InitTypeDef` 配置了三个关键参数：

- `GPIO_Pin = GPIO_Pin_10`：选择 PB10。
- `GPIO_Mode = GPIO_Mode_Out_PP`：设置为推挽输出。
- `GPIO_Speed = GPIO_Speed_50MHz`：设置 GPIO 输出速度等级。

主循环先调用 `GPIO_SetBits(GPIOB, GPIO_Pin_10)` 输出高电平，使 LED 熄灭并延时 100 ms；随后调用 `GPIO_ResetBits(GPIOB, GPIO_Pin_10)` 输出低电平，使 LED 点亮并延时 900 ms。由于 LED 的阳极连接 3.3V、阴极连接 PB10，该接法属于低电平有效。

`MyDelay.c` 中的 `Delay_us` 使用 SysTick，并按照 HCLK 为 72 MHz 的条件计算 `72 * xus` 的重装值。若系统时钟频率发生变化，需要同步调整该计算，否则实际延时时间会产生偏差。

## 构建与烧录

项目使用 VS Code 配合 EIDE 管理 STM32 工程，打开 `1.STM32F103-GPIO.code-workspace` 后可执行：

1. `build`：编译并链接项目。
2. `flash`：将已有编译产物烧录到设备。
3. `build and flash`：编译成功后直接烧录。
4. `rebuild`：清理后重新编译。
5. `clean`：删除编译产物。

烧录前请确认调试器已连接、目标芯片型号和 Flash/RAM 配置正确，并确认开发板与 LED 电路共地。
