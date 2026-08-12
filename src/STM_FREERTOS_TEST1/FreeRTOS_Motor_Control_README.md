# 🤖 STM32 Bare-Metal Robotics Control (FreeRTOS + HAL)

> **Real-time dual-motor control architecture using FreeRTOS message queues and hardware PWM on an STM32 Nucleo-F446RE.**

This README covers the base embedded implementation for controlling a high-current DC motor (via a BTS7960 driver) and a standard PWM servo using an event-driven RTOS architecture.

---

## 📋 Architecture Overview

This project separates hardware control from communication logic using FreeRTOS queues.

```
[ UART ISR ] ──> (Thread Flag) ──> [ UART_Task ]
                                         │
                                         ├──> [ DCQueue ] ──> [ DCMotor_Task ] ──> (TIM3 PWM)
                                         │
                                         └──> [ ServoQueue ] ──> [ Servo_Task ] ──> (TIM4 PWM)
```

## 🛠️ Hardware Setup

| Component | STM32 Pin | Timer/Channel | Notes |
| :--- | :--- | :--- | :--- |
| **DC Motor PWM (BTS7960)** | `PB4` | TIM3_CH1 | 65535 Max Period |
| **DC Motor DIR_L** | `PA11` | GPIO Output | |
| **DC Motor DIR_R** | `PA12` | GPIO Output | |
| **Servo PWM** | `PB6` | TIM4_CH1 | 50Hz (20ms) Period |
| **UART RX** | `PA3` (USART2) | ISR / IT | 115200 Baud |

### Important Power Warning (The Jumper Burnout)
If you are powering the STM32 from a heavy external 5V/12V source (like an LM2596 buck converter tied to a motor battery), you **MUST** move the `JP5` jumper on the Nucleo board from `U5V` to `E5V`.
* **Risk:** If `JP5` is left on `U5V`, the external battery will back-feed into the laptop's USB port, triggering an overcurrent shutdown or frying the ST-Link controller on the board.

---

## 🚨 Known Issues & Troubleshooting

### 1. Serial Monitor Shows Garbage or Nothing
* **Cause:** The `HAL_UART_Receive_IT` needs to be "primed" before the OS kernel starts.
* **Fix:** Ensure `HAL_UART_Receive_IT(&huart2, &rx_byte, 1);` is called in `main.c` right before `osKernelInitialize();`. Ensure the ISR callback re-arms the interrupt.

### 2. Servo Jitters or Moves Erratically
* **Cause:** The Timer Prescaler/Period is mathematically incorrect for 50Hz, or the map logic is outputting floats instead of integers.
* **Fix:** For a 50Hz signal on an 84MHz clock: Prescaler = `84-1`, Period = `20000-1`.
  * The angle calculation must be integer-safe: `1000 + ((angle * 1000) / 180)`.

### 3. Board Becomes Unresponsive / PC Does Not Detect USB
* **Cause:** A high-voltage short circuit (e.g., multimeter probe slipping) pushed 12V back into the STM32 logic pins, destroying the ST-Link chip's USB data lines.
* **Fix:** The ST-Link is dead. You must use an external USB-to-TTL serial adapter on pins `PA2/PA3` to read the monitor, or replace the Nucleo board.
