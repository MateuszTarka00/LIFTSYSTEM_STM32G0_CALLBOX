/*
 * canManager.c
 *
 *  Created on: Jan 19, 2026
 *      Author: mateo
 */

#include "canManager.h"
#include "fdcan.h"
#include "flash.h"

#define FIRST_SEND_ID	 0x200;
#define FIRST_RECEIVE_ID 0x80;

volatile uint32_t sendID = 0x200;
volatile uint8_t receiveID = 0x80;

volatile uint32_t sendIDTmp = 0x200;
volatile uint8_t receiveIDTmp = 0x80;

QueueHandle_t canRxQueue;

void CAN_InitRTOS(void)
{
    canRxQueue = xQueueCreate(16, sizeof(CAN_Message_t));
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef rxHeader;
    CAN_Message_t msg;
    BaseType_t hpw = pdFALSE;


    HAL_FDCAN_GetRxMessage(
        hfdcan,
        FDCAN_RX_FIFO0,
        &rxHeader,
		msg.data
    );

    if(msg.data[0] == receiveID) //Check if it is for me
    {
        xQueueSendFromISR(
            canRxQueue,
            &msg,
			hpw
        );

        portYIELD_FROM_ISR(hpw);
    }
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo1ITs)
{
    FDCAN_RxHeaderTypeDef rxHeader;
    CAN_Message_t msg;
    BaseType_t hpw = pdFALSE;


    HAL_FDCAN_GetRxMessage(
        hfdcan,
        FDCAN_RX_FIFO1,
        &rxHeader,
		msg.data
    );

    if(msg.data[0] == receiveID) //Check if it is for me
    {
        xQueueSendFromISR(
            canRxQueue,
            &msg,
			hpw
        );

        portYIELD_FROM_ISR(hpw);
    }
}

void processMessage(CAN_Message_t *msg)
{

}

void floorIDSubTask(void)
{
	static bool prevProgramFloor = false;
	bool programFloor = HAL_GPIO_ReadPin(PROGRAM_FLOOR_JMP_GPIO_Port, PROGRAM_FLOOR_JMP_Pin);

	if(HAL_GPIO_ReadPin(PROGRAM_FLOOR_JMP_GPIO_Port, PROGRAM_FLOOR_JMP_Pin) == GPIO_PIN_SET)
	{
		if(prevProgramFloor == false)
		{
			sendIDTmp = 0x200;
			receiveIDTmp = 0x80;
		}
	}
	else
	{
		if(prevProgramFloor)
		{
			sendID = sendIDTmp;
			receiveID = receiveIDTmp;

			flash_floorNumber.floorNumberIdReceive = receiveID;
			flash_floorNumber.floorNumberIdSend = sendID;
			saveValues();
		}
	}

	programFloor = prevProgramFloor;
}

