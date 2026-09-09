# STM32 工业智能化控制微型 PLC

STM32F4 工业控制板源码快照，覆盖工业 I/O、继电器与模拟量接口、RS485/Modbus
通信以及 CubeMX/Keil 工程组织方式。

## 内容

- `Design_Files`：STM32F4 工程、原理图/PCB 与器件资料
- `Code_Documentation`：工程说明和接口文档
- 支持将数字量、模拟量和串口通信模块组织成可扩展的微型 PLC 控制框架

仓库只保留源码、设计文件和文档，编译产物与本地 IDE 缓存已排除。构建时请按
工程说明配置对应 STM32 HAL/CMSIS 依赖。
