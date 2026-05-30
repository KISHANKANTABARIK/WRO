/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"  // <-- THIS FIXES THE OSDELAY AND QUEUE ERRORS
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/int32.h>
#include <rmw_microros/rmw_microros.h>

// Bring in your hardware timers for the encoder
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart2;

// Bring in your Queues from main.c
extern osMessageQueueId_t DCQueueHandle;
extern osMessageQueueId_t ServoQueueHandle;

void * microros_allocate(size_t size, void * state);
void microros_deallocate(void * pointer, void * state);
void * microros_reallocate(void * pointer, size_t size, void * state);
void * microros_zero_allocate(size_t number_of_elements, size_t size, void * state);
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

std_msgs__msg__Int32 msg_encoder;
std_msgs__msg__Int32 msg_speed;
std_msgs__msg__Int32 msg_dir;
std_msgs__msg__Int32 msg_servo;

// Callback functions
void speed_callback(const void * msgin) {
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  uint16_t speed_val = (uint16_t)msg->data;
  osMessageQueuePut(DCQueueHandle, &speed_val, 0, 0);
}

void dir_callback(const void * msgin) {
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  if (msg->data == 1) {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
  } else {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
  }
}

void servo_callback(const void * msgin) {
  const std_msgs__msg__Int32 * msg = (const std_msgs__msg__Int32 *)msgin;
  uint16_t angle_val = (uint16_t)msg->data;
  osMessageQueuePut(ServoQueueHandle, &angle_val, 0, 0);
}

// Transport functions (UPDATED FOR YOUR MICRO-ROS VERSION)
bool cubemx_transport_open(struct uxrCustomTransport * transport);
bool cubemx_transport_close(struct uxrCustomTransport * transport);
size_t cubemx_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
size_t cubemx_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */


// We define the function here in freertos.c to keep things clean!
/* USER CODE BEGIN Application */
void StartMicroROSTask(void *argument)
{
  // 1. Set up the transport
  rmw_uros_set_custom_transport(
    true, (void *) &huart2,
    cubemx_transport_open, cubemx_transport_close,
    cubemx_transport_write, cubemx_transport_read);

  // 2. Set up the memory allocator specifically for FreeRTOS (FROM YOUR README!)
  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
  freeRTOS_allocator.allocate = microros_allocate;
  freeRTOS_allocator.deallocate = microros_deallocate;
  freeRTOS_allocator.reallocate = microros_reallocate;
  freeRTOS_allocator.zero_allocate =  microros_zero_allocate;
  rcutils_set_default_allocator(&freeRTOS_allocator);

  rclc_support_t support;
  rcl_node_t node;
  rclc_executor_t executor;

  rcl_publisher_t pub_encoder;
  rcl_subscription_t sub_speed;
  rcl_subscription_t sub_dir;
  rcl_subscription_t sub_servo;

  // 3. Initialize micro-ROS (Wait until the Pi agent is connected - 1000ms timeout!)
  rmw_ret_t status = RMW_RET_ERROR;
  while(status != RMW_RET_OK) {
      status = rmw_uros_ping_agent(1000, 1);
      osDelay(100);
  }

  // 4. Create Node, Publisher, and Subscribers
  rclc_support_init(&support, 0, NULL, &freeRTOS_allocator);
  rclc_node_init_default(&node, "stm32_base_node", "", &support);

  rclc_publisher_init_default(&pub_encoder, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "/encoder_ticks");
  rclc_subscription_init_default(&sub_speed, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "/drive_motor_speed");
  rclc_subscription_init_default(&sub_dir, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "/drive_motor_dir");
  rclc_subscription_init_default(&sub_servo, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32), "/servo_angle");

  rclc_executor_init(&executor, &support.context, 3, &freeRTOS_allocator);
  rclc_executor_add_subscription(&executor, &sub_speed, &msg_speed, &speed_callback, ON_NEW_DATA);
  rclc_executor_add_subscription(&executor, &sub_dir, &msg_dir, &dir_callback, ON_NEW_DATA);
  rclc_executor_add_subscription(&executor, &sub_servo, &msg_servo, &servo_callback, ON_NEW_DATA);

  /* Infinite loop */
  for(;;)
  {
    rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));

    msg_encoder.data = (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
    rcl_publish(&pub_encoder, &msg_encoder, NULL);

    osDelay(10);
  }
}


/* USER CODE END Application */

