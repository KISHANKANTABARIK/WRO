graph TD
  %% Define Styles
  classDef pi fill:#e83e8c,stroke:#fff,stroke-width:2px,color:#fff;
  classDef stm fill:#0d6efd,stroke:#fff,stroke-width:2px,color:#fff;
  classDef hardware fill:#6c757d,stroke:#fff,color:#fff;
  classDef topic fill:#ffc107,stroke:#333,color:#000;

  subgraph PI4 [Raspberry Pi 4 - ROS 2 Humble]
    subgraph Sensors_Pi [Pi Hardware]
      cam[Studica 3D Depth Cam]:::hardware
      lidar[Studica 360 LiDAR 70500]:::hardware
      imu[I2C IMU Sensor]:::hardware
    end

    subgraph Drivers [ROS 2 Driver Nodes]
      cam_node(camera_node):::pi
      lidar_node(lidar_node):::pi
      imu_node(imu_node):::pi
    end

    subgraph Brain [WRO Autonomy Stack]
      vision(wro_vision_node):::pi
      state(wro_state_machine_node):::pi
      nav(wro_nav_node):::pi
    end

    agent(micro_ros_agent):::pi
  end

  subgraph STM32 [STM32 Nucleo-F446RE - FreeRTOS]
    base_node(stm32_base_node):::stm
    
    subgraph Sensors_Actuators [Low-Level Hardware]
      motor[BTS7960 Drive Motor]:::hardware
      servo[Steering Servo]:::hardware
      encoder[Hardware Encoder]:::hardware
      us_ir[2x Ultrasonic & 2x IR Sensors]:::hardware
    end
  end

  %% Pi 4 Internal Connections
  cam --> cam_node
  lidar --> lidar_node
  imu --> imu_node

  cam_node -- /camera/color/image_raw --> vision
  lidar_node -- /scan --> nav
  imu_node -- /imu/data --> nav
  
  vision -- /perception/lane_error <br/> /perception/obstacles --> nav
  state -- /system/mission_state --> nav

  %% The micro-ROS Bridge
  nav -- /drive_motor_speed <br/> /servo_angle --> agent
  agent <==>|USB / UART Bridge| base_node
  agent -- /sensors/ultrasound <br/> /sensors/ir <br/> /sensors/encoder_ticks --> nav
  
  %% STM32 Internal Connections
  us_ir --> base_node
  encoder --> base_node
  base_node --> motor
  base_node --> servo
