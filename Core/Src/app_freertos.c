/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "canInputOutputDefinitions.h"
#include "buttons.h"
#include "canManager.h"

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

/* USER CODE END Variables */
/* Definitions for ButtonsTaskT */
osThreadId_t ButtonsTaskTHandle;
const osThreadAttr_t ButtonsTaskT_attributes = {
  .name = "ButtonsTaskT",
  .priority = (osPriority_t) osPriorityNormal2,
  .stack_size = 512 * 4
};
/* Definitions for CanTranciverT */
osThreadId_t CanTranciverTHandle;
const osThreadAttr_t CanTranciverT_attributes = {
  .name = "CanTranciverT",
  .priority = (osPriority_t) osPriorityNormal3,
  .stack_size = 1024 * 4
};
/* Definitions for CanReceiverT */
osThreadId_t CanReceiverTHandle;
const osThreadAttr_t CanReceiverT_attributes = {
  .name = "CanReceiverT",
  .priority = (osPriority_t) osPriorityAboveNormal,
  .stack_size = 512 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void ButtonsTask(void *argument);
void CanTranciver(void *argument);
void CanReceiver(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);
void vApplicationDaemonTaskStartupHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
void vApplicationTickHook( void )
{
   /* This function will be called by each tick interrupt if
   configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h. User code can be
   added here, but the tick hook is called from an interrupt context, so
   code must not attempt to block, and only the interrupt safe FreeRTOS API
   functions can be used (those that end in FromISR()). */
}
/* USER CODE END 3 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

/* USER CODE BEGIN DAEMON_TASK_STARTUP_HOOK */
void vApplicationDaemonTaskStartupHook(void)
{
}
/* USER CODE END DAEMON_TASK_STARTUP_HOOK */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of ButtonsTaskT */
  ButtonsTaskTHandle = osThreadNew(ButtonsTask, NULL, &ButtonsTaskT_attributes);

  /* creation of CanTranciverT */
  CanTranciverTHandle = osThreadNew(CanTranciver, NULL, &CanTranciverT_attributes);

  /* creation of CanReceiverT */
  CanReceiverTHandle = osThreadNew(CanReceiver, NULL, &CanReceiverT_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_ButtonsTask */
/**
  * @brief  Function implementing the ButtonsTaskT thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_ButtonsTask */
void ButtonsTask(void *argument)
{
  /* USER CODE BEGIN ButtonsTask */
  /* Infinite loop */
  for(;;)
  {
	buttonsSubTask();
    osDelay(1);
  }
  /* USER CODE END ButtonsTask */
}

/* USER CODE BEGIN Header_CanTranciver */
/**
* @brief Function implementing the CanTranciverT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CanTranciver */
void CanTranciver(void *argument)
{
  /* USER CODE BEGIN CanTranciver */
  /* Infinite loop */
  for(;;)
  {
	  tranciverFunction();
    osDelay(1);
  }
  /* USER CODE END CanTranciver */
}

/* USER CODE BEGIN Header_CanReceiver */
/**
* @brief Function implementing the CanReceiverT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_CanReceiver */
void CanReceiver(void *argument)
{
  /* USER CODE BEGIN CanReceiver */
  /* Infinite loop */
  for(;;)
  {
	CAN_Message_t msg;
	if(xQueueReceive(canRxQueue, &msg, portMAX_DELAY) == pdTRUE)
	{
		processMessage(&msg);
	}
  }
  /* USER CODE END CanReceiver */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
