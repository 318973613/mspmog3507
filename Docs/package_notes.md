# MSPM0G3507 SmartCar FreeRTOS 工程分发说明

## 导入方式

1. 在 CCS 中选择 `Import Existing CCS Project`。
2. 选择本工程根目录 `MSPM0G3507_SmartCar_FreeRTOS`。
3. 确认本机已安装：
   - MSPM0 SDK `2.10.00.04`
   - SysConfig `1.28.0`
   - TI ArmClang `5.1.1.LTS`
4. 导入后直接执行 `Build Project`，CCS 会重新生成 `Debug` 构建目录和 `ti_msp_dl_config.c/h`。

## 重要约束

- 不要手改 `Debug/ti_msp_dl_config.c/h`，这些文件会被 SysConfig 重新生成。
- 引脚修改必须通过 `mspm0g3507_smartcar_freertos.syscfg`，当前优化没有改 PA/PB 引脚分配。
- 业务逻辑在 `main.c`、`APP/src`、`APP/header`、`Drivers/MSPM0`、`rtos_*` 等手写文件中，重新生成 HAL 不会覆盖这些文件。

## 当前上电行为

- 上电默认 `IDLE`，电机刹车，不会自动跑。
- K1：启动/停止循迹。
- K2：非运行态采集白底灰度校准。
- K3：非运行态采集黑线校准，运行态急停。
- OLED 显示模式、校准状态、左右轮速度、灰度数字量、黑点数量和循迹误差。

## 未接入项

- 当前工程没有 JY61P/IMU 实际引脚和协议解析，只保留了角度 PID 相关变量/函数雏形。
- 如需加入 JY61P，必须先确认实际 UART/I2C 接线、波特率/协议，再增加驱动和任务。
