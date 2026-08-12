# WRO Future Engineers 2026 - [Your Team Name]

## 1. System Architecture
Our robot uses a hierarchical control pipeline:
* **High-Level Controller:** Raspberry Pi 4 running ROS 2 (Humble/Foxy) and the `micro_ros_agent`.
* **Low-Level Microcontroller:** STM32F446RE running micro-ROS over UART/USB to generate high-frequency PWM signals.
* **Actuation:**
  * DC Motor driven via BTS7960 High-Current H-Bridge Driver (12V Supply).
  * DC Steering Servo driven via STM32 PWM output (5V Supply).

---

## 2. Hardware Signal Inversion Circuit (BTS7960 Interface)
Because the STM32F446RE operates at 3.3V logic and the BTS7960 requires higher high-level input thresholds, an NPN transistor level shifter / inverter circuit is used between the STM32 PWM output pins (`PA0`, `PA1`) and the driver inputs (`RPWM`, `LPWM`).

### Circuit Behavior & Logic Inversion
* **STM32 Pin HIGH (3.3V)** → Transistor turns ON → Output pulled LOW (~0V).
* **STM32 Pin LOW (0V)** → Transistor turns OFF → Output pulled HIGH (5V via 1kΩ resistor).

![Circuit Schematic](doc/circuit_schematic.png)

### Software Correction Formula
To compensate for active-low signal inversion in software:
$$\text{Output Compare Value (CCR)} = \text{Timer Period (ARR)} - \text{Target PWM Duty}$$

---

## 3. Firmware Code Snippet (STM32 Control Logic)

```c
// Timer Period ARR = 4199 (20kHz PWM @ 84MHz clock)
#define MAX_PWM 4199

void set_dc_motor_speed(int16_t speed) {
    if (speed > MAX_PWM) speed = MAX_PWM;
    if (speed < -MAX_PWM) speed = -MAX_PWM;

    if (speed > 0) {
        // Forward: Software duty cycle inverted for transistor stage
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, MAX_PWM - speed); // RPWM
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, MAX_PWM);         // LPWM OFF
    } else if (speed < 0) {
        // Reverse
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, MAX_PWM);         // RPWM OFF
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, MAX_PWM - (-speed)); // LPWM
    } else {
        // Brake / Stop (Both outputs OFF = 100% duty at STM32 pins)
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, MAX_PWM);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, MAX_PWM);
    }
}
