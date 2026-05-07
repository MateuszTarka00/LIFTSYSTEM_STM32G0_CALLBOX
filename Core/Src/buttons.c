/*
 * buttons.c
 *
 *  Created on: 6 maj 2026
 *      Author: mateo
 */

#include "buttons.h"
#include "task.h"
#include "cmsis_os.h"
#include "canManager.h"

#define BUTTON_UP		0x01
#define BUTTON_DOWN		0x02

bool upRequest = false;
bool downRequest = false;

static volatile uint8_t buttonsPressed = 0;
static volatile uint8_t buttonsPulled = 0;

InputDebounce_t btnUp = { 0, 0, 0 };
InputDebounce_t btnDown = { 0, 0, 0 };

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	BaseType_t hpw = pdFALSE;
	switch(GPIO_Pin)
	{
		case BUTTON_UP_Pin:
			buttonsPressed |= BUTTON_UP;
			break;
		case BUTTON_DOWN_Pin:
			buttonsPressed |= BUTTON_DOWN;
			break;
	}
	vTaskNotifyGiveFromISR(ButtonsTaskTHandle, &hpw);
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
	BaseType_t hpw = pdFALSE;
	switch(GPIO_Pin)
	{
		case BUTTON_UP_Pin:
			buttonsPulled |= BUTTON_UP;
			break;
		case BUTTON_DOWN_Pin:
			buttonsPulled |= BUTTON_DOWN;
			break;
	}
	vTaskNotifyGiveFromISR(ButtonsTaskTHandle, &hpw);
}


void buttonsSubTask(void)
{
	TickType_t now = xTaskGetTickCount();

	if(buttonsPressed)
	{
		if(buttonsPressed & BUTTON_UP)
		{
			btnUp.lastTick = now;
			btnUp.pendingPress = true;
			buttonsPressed &= ~BUTTON_UP;
		}

		if(buttonsPressed & BUTTON_DOWN)
		{
			btnDown.lastTick = now;
			btnDown.pendingPress = false;
			buttonsPressed &= ~BUTTON_DOWN;
		}

	}

	if(buttonsPulled)
	{
		if(buttonsPulled & BUTTON_UP)
		{
			btnUp.lastTick = now;
			btnUp.pendingRelease = true;
			buttonsPulled &= ~BUTTON_UP;
		}

		if(buttonsPulled & BUTTON_DOWN)
		{
			btnDown.lastTick = now;
			btnDown.pendingRelease = true;
			buttonsPulled &= ~BUTTON_DOWN;
		}
	}

    if(btnUp.pendingPress &&
       (now - btnUp.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_UP_GPIO_Port, BUTTON_UP_Pin) == GPIO_PIN_SET)
        {
        	if(HAL_GPIO_ReadPin(PROGRAM_FLOOR_JMP_GPIO_Port, PROGRAM_FLOOR_JMP_Pin) == GPIO_PIN_SET)
        	{
        		floorIncrease();
        	}
        	else
        	{
        		upRequest = true;
        	}
        }
        btnUp.pendingRelease = false;
    }

    if(btnDown.pendingPress &&
       (now - btnDown.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_SET)
        {
        	if(HAL_GPIO_ReadPin(PROGRAM_FLOOR_JMP_GPIO_Port, PROGRAM_FLOOR_JMP_Pin) == GPIO_PIN_RESET)
        	{
        		downRequest = true;
        	}
        }
        btnDown.pendingRelease = false;
    }

    if(btnUp.pendingRelease && !btnUp.pendingPress &&
       (now - btnUp.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_UP_GPIO_Port, BUTTON_UP_Pin) == GPIO_PIN_RESET)
        {
           upRequest = false;
        }
        btnUp.pendingRelease = false;
    }

    if(btnDown.pendingRelease && !btnDown.pendingPress &&
       (now - btnDown.lastTick) >= pdMS_TO_TICKS(DEBOUNCE_TIME_MS))
    {
        if(HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_RESET)
        {
        	downRequest = false;
        }
        btnDown.pendingRelease = false;
    }

}
