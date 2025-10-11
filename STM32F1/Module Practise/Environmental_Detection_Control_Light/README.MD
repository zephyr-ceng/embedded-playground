## 项目说明

### 实现效果

检测环境中的温度、湿度、光照进行相应的自动灯控

-   OLED 实时显示温度、光照值
-   光线变暗自动开启 LED，光线变亮自动关闭 LED（红色）
-   温度高于 20 摄氏度，以 50Hz 闪烁 LED，低于 20 摄氏度以 1KHz 闪缩 LED（绿色）
-   串口实时发送温度和光照
-   按键手动采集当前湿度显示至 LED

### 外设分布

-   ADC 数据采集
-   IIC 驱动 OLED
-   GPIO 控制 LED
-   GPIO 按键检测
-   USART 数据传输
-   TIM PWM 呼吸灯（50Hz 1KHz）

### 引脚分布

| 模块                  | 接口                   | 说明               |
| --------------------- | ---------------------- | ------------------ |
| OLED 屏               | I2C (PB6=SCL, PB7=SDA) | 显示温度/亮度/状态 |
| 温度传感器（如 LM35） | ADC_IN0 (PA0)          | 模拟信号输入       |
| 光敏电阻              | ADC_IN1 (PA1)          | 光照检测           |
| LED 灯                | GPIO (PC13)            | 状态显示或亮度指示 |
| 按键                  | GPIO (PB0)             | 模式切换           |
| 串口模块              | USART1 (PA9, PA10)     | 数据上传/调试      |

### 学习目的

-   DMA 驱动下的高效 ADC & USART 通信
-   多模块任务调度（用 TIM 控制）
-   模块封装与回调机制（学习 SPL 框架思想）
<!-- - 数据采集滤波算法（滑动平均、中值滤波） -->
-   串口协议解析（帧头帧尾 + 校验）
-   OLED 动态刷新优化（局部刷新）

### 后续拓展

ESP8266 上传当前采集的温湿度、光照等数据，然后远程手动控制 LED

项目改写为自动浇花系统（采集土壤的温湿度、光照，控制浇水、开关灯）

## 目录结构说明

```yaml
Environmental_Detection_Control_Light
├─ .clang-format
├─ .eide # EIDE 配置
│  ├─ eide.yml
│  ├─ env.ini
│  └─ files.options.yml
├─ .pack # STM32F1 芯片驱动包
│  └─ Keil
│     └─ STM32F1xx_DFP.2.3.0
├─ Environmental_Detection_Control_Light.code-workspace # EIDE 工程文件
├─ hal  # 外设驱动文件
│  └─ STM32F10x_StdPeriph_Driver
├─ lib # cmsis 文件（ARM Cortex 微控制器软件接口标准）
├─ README.MD
├─ RTE_Components.h
├─ src # 详细代码文件
│  ├─ Driver
│  │  ├─ inc
│  │  │  ├─ MyDelay.h
│  │  │  ├─ OLED.h
│  │  │  └─ OLED_Font.h
│  │  └─ src
│  │     ├─ MyDelay.c
│  │     └─ OLED.c
│  ├─ main.c
│  ├─ startup_stm32f10x_md.s # 启动文件
│  ├─ stm32f10x_conf.h # 外设库配置文件
│  ├─ stm32f10x_it.c # 中断服务函数实现文件
│  └─ stm32f10x_it.h # 中断函数声明头文件
└─ stm32f1x_64KB_flash.ld # 链接脚本（定义程序的 内存布局，代码段、数据段、堆栈分布）

```
