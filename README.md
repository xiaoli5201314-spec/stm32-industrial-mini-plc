# STM32 工业智能化控制微型 PLC（MAXREFDES61 采样控制固件快照）

这是一个基于 **STM32F415RGTx / STM32F4 HAL** 的 MAXREFDES61# 参考设计固件快照。
代码提供串口菜单、四路模拟输入选择、MAX1301 ADC 初始化、连续采样和定长块采样，
并通过 SPI 与采样器件交换数据。仓库名称沿用简历中的“微型 PLC”项目定位，但当前
公开内容本质上是一个可扩展的工业模拟量采集/控制板固件，不声称包含完整 PLC 梯形图
运行时或量产硬件验证结果。

## 项目定位

固件将 STM32 外设初始化、MAX1301 设备配置、定时采样和串口人机交互组合为一个可运行的
采样控制闭环：

```text
UART1 921600 ── 菜单/样本输出 ──► STM32F415 ── SPI1 ──► MAX1301 ADC
                                      │  │
                                      │  ├─ TIM1 更新中断：定时触发采样
                                      │  ├─ SPI2：板级从设备接口（CubeMX 配置）
                                      │  └─ I2C1：板级扩展/器件接口
                                      └─ GPIO：片选、输入状态与调试信号
```

启动后，`main.c` 初始化 GPIO、I2C1、SPI1、SPI2、TIM1 和 USART1，调用 `ADCInit()`
写入 MAX1301 输入范围配置，然后在终端显示交互菜单。用户可以选择：

- 连续采样：周期性读取指定 ADC 通道并把数据打印到终端；
- 块采样：选择采样速率和样本数量，使用 TIM1 中断驱动 SPI 读取，把结果存入
  `g_auADCData[65536]`，完成后可回读并打印样本；
- 按 `ESC` 中止采样或样本回读，回到主菜单。

## 已实现的核心功能

### 设备初始化与量程配置

`maximDeviceSpecificUtilities.c` 中的 `ADCInit()` 通过 SPI1 向 MAX1301 写入初始化寄存器。
源码注释给出了四个输入的目标范围：AIN0/1 为 ±10 V、AIN2 为 0–5 V、AIN3 为 0–10 V。
具体量程仍取决于 MAXREFDES61 板卡的模拟前端和电源条件，不能仅凭固件推导最终端子精度。

### 定时块采样

`StartSampling(channel, sampleSize, sampleRate, samples)` 完成一次块采样：

1. 根据通道生成 SPI 读取命令；
2. 按菜单选择配置 TIM1 周期（源码选项为 1 ksps、10 ksps、25 ksps、50 ksps）；
3. 在 `HAL_TIM_PeriodElapsedCallback()` 中拉低片选、发送命令、读取 SPI 数据并写入样本数组；
4. 达到目标样本数或用户中止后停止定时器并返回实际样本数。

采样数据使用 `uint16_t` 保存，数组在 `main.c` 中映射到 `0x20000000`。这个地址布局是原始
参考工程的一部分，换用不同容量/链接脚本的 MCU 时需要重新评估 SRAM 区域，不能直接照搬。

### 终端菜单与可观测性

`menu.c` 提供清屏、版本横幅、通道/速率/样本大小菜单和按键解析。USART1 通过 `retarget.c`
重定向 `printf/getchar`，便于用串口终端观察采样过程和导出文本数据。菜单中的块采样大小为
8K、16K、32K 或 64K；实际可用上限受 SRAM、编译器链接布局和堆栈需求限制。

## 硬件与外设配置

| 项目 | 当前配置 |
| --- | --- |
| MCU | STM32F415RGTx，LQFP64，STM32F4 HAL |
| SPI1 | 主机，16-bit 数据，源码配置约 3.125 Mbit/s；连接 MAX1301/采样器件 |
| SPI2 | 从机接口，CubeMX 配置约 12.5 Mbit/s |
| I2C1 | PB6/PB7，开漏上拉 |
| USART1 | PA9/PA10，921600 baud；同时承担菜单输入与样本输出 |
| TIM1 | 内部时钟，更新中断用于定时采样 |
| GPIO | PA8 作为 ADC 片选；其余引脚和调试/扩展接口见 `.ioc` |
| 时钟 | `MAXREFDES61.ioc` 中记录 HSE/PLL 与 APB 分频参数 |

引脚、时钟和外设初始化的权威来源是 [`Design_Files/MAXREFDES61.ioc`](Design_Files/MAXREFDES61.ioc)
及 `Design_Files/Inc`、`Design_Files/Src` 中的 HAL 文件。硬件设计图、BOM 和外部模拟
前端不在本快照中，使用前请核对 MAXREFDES61 板卡资料。

## 目录结构

```text
Design_Files/
  MAXREFDES61.ioc             # STM32CubeMX 外设/引脚配置
  Inc/                         # HAL 外设头文件
  Src/
    main.c                     # 初始化、菜单状态机和采样主流程
    maximDeviceSpecificUtilities.c/.h
                               # MAX1301 配置、连续/块采样和 TIM1 回调
    menu.c/.h                  # 串口菜单与按键解析
    utilities.c/.h             # 延时与通用工具
    gpio.c, spi.c, i2c.c,
    tim.c, usart.c              # CubeMX/HAL 外设初始化
    retarget.c                 # printf/getchar 串口重定向
  Drivers/
    STM32F4xx_HAL_Driver/      # HAL 驱动源码
    CMSIS/                     # CMSIS/RTOS 头文件
  Projects/MDK-ARM/            # Keil MDK 工程及启动文件
Code_Documentation/
  MainPage.html, html/         # Doxygen 生成的 API/流程图文档
```

## 构建与运行

### Keil MDK

1. 使用 Keil MDK 打开 `Design_Files/Projects/MDK-ARM/MAXREFDES61.uvproj`。
2. 确认 STM32F4 Device Family Pack、编译器版本和下载器设置；工程的原始配置来自
   MDK-ARM 4.x/CubeMX 4.x 时代，较新版本可能需要迁移工程格式。
3. 编译并下载后，通过 USART1 连接终端（921600 baud，8-N-1；流控按板卡连接配置）。
4. 上电后看到版本横幅，按菜单选择通道和采样模式；块采样完成后可将打印数据保存为文本。

### CubeMX 重新生成

`MAXREFDES61.ioc` 可用于查看或重新生成 HAL 初始化代码。重新生成前请备份 `Src` 中的
USER CODE 区域和 MAXIM 设备驱动；直接覆盖可能丢失 `ADCInit()`、采样回调和菜单逻辑。

## 验证边界与注意事项

- 当前仓库没有在本环境中执行真实 STM32 编译、下载或示波器采样验证；README 中的采样率
  和输入范围来自源码注释/配置，而不是本次运行实测指标。
- 采样使用忙等待和中断回调，长时间输出 64K 样本会占用串口带宽；不要把终端打印路径
  当作实时数据采集链路。
- `StartSampling()` 直接操作 SPI 数据寄存器并假定 MAXREFDES61 硬件时序；换用其他 ADC、
  电压范围或 SPI 拓扑必须重新校验命令格式、片选极性和时钟。
- 该快照没有实现 PLC 网络协议、Modbus 寄存器表、远程升级或安全联锁；这些属于后续产品
  化工作，不应从项目名称推断已经具备。

## 许可证与第三方说明

源码文件保留了 Maxim Integrated Products、STMicroelectronics 和 STM32 HAL 的原始版权
与免责声明。`Code_Documentation/MAXREFDES61_Code_Documentation.pdf` 及 `html/` 是随参考
设计生成的文档。使用或再分发时请遵守 MAXREFDES61 参考设计、STM32 HAL/CMSIS 及其他随
工程提供的第三方许可条款；本仓库不改变这些上游许可证，也不代表与相关厂商存在商业关联。
