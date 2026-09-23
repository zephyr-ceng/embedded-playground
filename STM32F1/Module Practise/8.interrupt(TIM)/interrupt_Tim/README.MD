## 目录结构说明
```text
├─.eide # EIDE配置
├─.pack # PACK包配置
│  └─Keil
│      └─STM32F1xx_DFP.2.3.0
│          ├─Boards
│          ├─Debug
│          ├─Device
│          │  ├─Include
│          │  ├─Source
│          │  │  └─ARM
│          │  └─StdPeriph_Driver
│          │      ├─inc
│          │      ├─src
│          │      └─templates
│          ├─Documents
│          ├─Flash
│          │  └─STM32F10x
│          ├─RTE_Driver
│          │  └─Config
│          └─SVD
├─.vscode # Vscode配置
├─build  # 项目构建目录
│  └─Debug
├─hal # 外设驱动
│  └─STM32F10x_StdPeriph_Driver
│      ├─inc
│      └─src
├─lib # 内核驱动
│  └─cmsis
└─src # 核心代码
    ├─BSP_Drive # 底层驱动 
    │  ├─inc
    │  └─src
    └─Modules # 模块驱动
        ├─inc
        └─src
```