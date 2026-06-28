# RTOS Clocking Notes

- Current clock tree: 80 MHz CPU clock from the configured system clock tree.
- External crystal: not enabled in this project. No clock pins were changed.
- RTOS tick source: TIMG12, 1 kHz, no GPIO pin output.
- TIMG12 clock source: BUSCLK. With the current SysConfig UDIV setting, BUSCLK is 40 MHz and the reload value is 39999.
- Motor control cadence: 20 ms task period. Encoder speed calculation uses the same 20 ms sample period.

Using TIMG12 makes the RTOS tick independent from Cortex-M SysTick and leaves SysTick unused. Long-term timing accuracy still follows the configured system clock source. If the board is later confirmed to have a populated external crystal, switch the SysConfig clock tree first, then keep TIMG12 as the RTOS tick timer.
