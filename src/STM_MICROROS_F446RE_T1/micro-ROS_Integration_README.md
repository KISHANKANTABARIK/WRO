# 🤖 micro-ROS on STM32 (FreeRTOS Node)

> **Bridging a bare-metal FreeRTOS motor controller to a ROS 2 Humble network using micro-ROS and a Raspberry Pi.**

This architecture replaces the raw UART string parser with a native micro-ROS node. The STM32 subscribes directly to ROS 2 topics (`/drive_motor_speed`, `/servo_angle`) and funnels the integers straight into the existing FreeRTOS hardware queues.

---

## 📋 Architecture

```
[ Raspberry Pi 4 ]                 [ STM32 Nucleo-F446RE ]
ROS 2 Humble                       FreeRTOS + micro-ROS

$ ros2 topic pub ────(USB)────> [ it_transport.c ] ──> [ microROS_Task ]
/drive_motor_speed                                         │
                                                           ├──> [ DCQueue ]
                                                           └──> [ ServoQueue ]
```

## 🛠️ Critical CubeMX Configurations

To prevent the STM32 from freezing on boot when running micro-ROS, these specific settings must be applied in `.ioc`:

1. **SysTick Conflict:** FreeRTOS must use `TIM1` as its Timebase Source (System Core -> SYS).
2. **Memory Starvation:** The `microROS_Task` stack size must be increased from 128 words to **`3000`** words (Middleware -> FREERTOS).
3. **Interrupt Transport:** `USART2 global interrupt` MUST be checked in the NVIC settings to allow the Pi to handshake with the STM32.

---

## 🚨 Integration Challenges & Fixes

### 1. Linker Error: "Multiple Definition of..."
* **Cause:** Copying `it_transport.c` or memory allocator `.c` files directly into `Core/Src` while simultaneously telling the compiler to build the `extra_sources` folder. The compiler builds two copies of the same functions.
* **Fix:** Delete the duplicates from `Core/Src`. Ensure you only unlock the `extra_sources` folder and uncheck "Exclude from build" for `it_transport.c` specifically.

### 2. Phantom Error: "Build Failed" (But the .elf is generated)
* **Cause:** The Eclipse-based IDE misinterprets a standard warning (`_gettimeofday is not implemented`) as a fatal error.
* **Fix:** If the `arm-none-eabi-size` table prints in the console showing the memory footprint, the code **did** compile successfully. Ignore the "Build Failed" text and flash the board.

### 3. Agent Stuck on "running..." (No Session Established)
* **Cause A (Handshake Timeout):** The STM32 booted too fast and the Pi was not ready, or the ping timeout was too aggressive.
  * **Fix:** Change the ping loop to `rmw_uros_ping_agent(1000, 1)` (1000ms timeout). Press the physical **RESET** button on the STM32 *after* the agent says `running...`.
* **Cause B (Missing Allocators):** FreeRTOS crashed because micro-ROS tried to use standard `malloc()`.
  * **Fix:** Ensure the custom allocators (`microros_allocate`, etc.) are defined and linked via `rcutils_set_default_allocator(&freeRTOS_allocator);` before the ping loop starts.

### 4. Agent Crash: "Serial Port Not Found, Error 2"
* **Cause:** You pressed the RESET button on the Nucleo board while the Agent was actively streaming data. This briefly cuts power to the ST-Link USB chip, causing Linux to lose the `/dev/ttyACM0` mount.
* **Fix:** Restart the Agent command on the Pi.

## 🚀 Running the Node

1. Flash the STM32 and plug it into the Pi via USB.
2. On the Pi, source the workspace: `source ~/microros_ws/install/local_setup.bash`
3. Start the agent: `ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM0 -b 115200 -v6`
4. Press RESET on the STM32. Wait for `Session established`.
5. In a new terminal, drive the robot:
   `ros2 topic pub --once /drive_motor_speed std_msgs/msg/Int32 "{data: 25000}"`
