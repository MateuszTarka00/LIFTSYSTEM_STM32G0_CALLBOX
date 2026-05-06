/*
 * canManager.c
 *
 *  Created on: Jan 19, 2026
 *      Author: mateo
 */

#include "canManager.h"
#include "fdcan.h"

#define FIRST_SEND_ID	 0x200;
#define FIRST_RECEIVE_ID 0x80;

volatile uint32_t sendID = 0x200;
volatile uint32_t receiveID = 0x80;

volatile uint32_t sendIDTmp = 0x200;
volatile uint32_t receiveIDTmp = 0x80;

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

