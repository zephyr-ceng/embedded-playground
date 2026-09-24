## GPIO端口使用之点灯（推挽输出）

### 板载 LED 接线

板载 LED 采用 3V3 供电，GPIO 通过 1 kΩ 电阻连接 LED 阴极，属于低电平有效：

![alt text](image.png)
| LED | 颜色 | GPIO 引脚 | 定时器复用功能 | 接线 |
| --- | --- | --- | --- | --- |
| LED1 | 红色 | PA1 | TIM2_CH2 | 3V3 -> LED1 -> 1 kΩ -> PA1 |
| LED2 | 绿色 | PA2 | TIM2_CH3 | 3V3 -> LED2 -> 1 kΩ -> PA2 |
| LED3 | 蓝色 | PA3 | TIM2_CH4 | 3V3 -> LED3 -> 1 kΩ -> PA3 |

- GPIO 输出高电平：LED 熄灭
- GPIO 输出低电平：LED 点亮
