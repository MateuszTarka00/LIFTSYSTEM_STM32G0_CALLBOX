/*
 * canManager.c
 *
 *  Created on: Jan 19, 2026
 *      Author: mateo
 */

#include "canManager.h"
#include "fdcan.h"
#include "flash.h"
#include "canInputOutputDefinitions.h"
#include "lights.h"
#include "buttons.h"

#define FIRST_SEND_ID	 0x200
#define FIRST_RECEIVE_ID 0x80

volatile uint32_t sendID = 0x200;
volatile uint8_t receiveID = 0x80;

volatile uint32_t sendIDTmp = 0x200;
volatile uint8_t receiveIDTmp = 0x80;

static bool connectedToMaster = false;

QueueHandle_t canRxQueue;

static uint32_t FDCAN_BytesToDLC(uint8_t len)
{
    switch (len)
    {
        case 0: return FDCAN_DLC_BYTES_0;
        case 1: return FDCAN_DLC_BYTES_1;
        case 2: return FDCAN_DLC_BYTES_2;
        case 3: return FDCAN_DLC_BYTES_3;
        case 4: return FDCAN_DLC_BYTES_4;
        case 5: return FDCAN_DLC_BYTES_5;
        case 6: return FDCAN_DLC_BYTES_6;
        case 7: return FDCAN_DLC_BYTES_7;
        case 8: return FDCAN_DLC_BYTES_8;
        default: return FDCAN_DLC_BYTES_8; // safety fallback
    }
}

void FDCAN_Send(uint16_t id, uint8_t *data, uint8_t len)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.DataLength = FDCAN_BytesToDLC(len);
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan2) == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(1)); // yield CPU
    }

    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, data);
}


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

static void setCanLedOK(bool onOFF)
{
	HAL_GPIO_WritePin(CAN_OK_GPIO_Port, CAN_OK_Pin, onOFF);
}

static void setCanLedFault(bool onOFF)
{
	HAL_GPIO_WritePin(CAN_FAULT_GPIO_Port, CAN_FAULT_Pin, onOFF);
}


void CAN_UpdateLEDs(void)
{
    static uint32_t lastBlink = 0;
    static uint8_t blinkState = 0;

    uint32_t now = HAL_GetTick();

    FDCAN_ProtocolStatusTypeDef status;
    HAL_FDCAN_GetProtocolStatus(&hfdcan2, &status);

    /* --- BUS OFF --- */
    if (status.BusOff)
    {
    	setCanLedOK(false);
    	setCanLedFault(true);
        return;
    }

    /* --- ERROR PASSIVE / WARNING --- */
    if (status.ErrorPassive || status.Warning)
    {
        if ((now - lastBlink) > 500)
        {
            lastBlink = now;
            blinkState ^= 1;
        }

        if (blinkState)
        {
        	setCanLedOK(true);
        	setCanLedFault(true);
        }
        else
        {
        	setCanLedOK(false);
        	setCanLedFault(false);
        }
        return;
    }

    /* --- ERROR ACTIVE (NORMAL OPERATION) --- */
    setCanLedFault(false);

    /* Blink green if traffic present */

    	if(connectedToMaster != true)
    	{
    	    if ((now - lastBlink) > 200)
    	    {
    	        lastBlink = now;
    	        blinkState ^= 1;
    	    }

    	    if (blinkState)
    	    	setCanLedOK(true);
    	    else
    	    	setCanLedOK(false);

    	    return;
    	}


    setCanLedOK(true);

}

void processMessage(CAN_Message_t *msg)
{
	switch(msg->data[2])
	{
		case DOWN_BUTTON_THIRD_BYTE_CONST_RX:
			turnOnOffLight(&downLight, true);
			break;
		case UP_BUTTON_THIRD_BYTE_CONST_RX:
			turnOnOffLight(&upLight, true);
			break;
		case DOWN_BUTTON_THIRD_BYTE_BLINK_RX:
			startLightBlink(&downLight);
			break;
		case UP_BUTTON_THIRD_BYTE_BLINK_RX:
			startLightBlink(&upLight);
			break;
		default:
			turnOnOffLight(&upLight, false);
			turnOnOffLight(&downLight, false);
	}
}


void tranciverFunction(void)
{
	uint8_t message[4] = {0x00, 0x0F, 00, 00};

	if(upRequest && downRequest)
	{

	}
	else if(upRequest)
	{
		message[2] = UP_BUTTON_THIRD_BYTE_TX;
		message[3] = inputCanLastByte[sendID - FIRST_SEND_ID][1];

		FDCAN_Send(sendID, message, CAN_MESSAGE_SIZE);
	}
	else if(downRequest)
	{
		message[2] = UP_BUTTON_THIRD_BYTE_TX;
		message[3] = inputCanLastByte[sendID - FIRST_SEND_ID][2];

		FDCAN_Send(sendID, message, CAN_MESSAGE_SIZE);
	}
	else
	{
		message[2] = UP_BUTTON_THIRD_BYTE_TX;
		message[3] = inputCanLastByte[sendID - FIRST_SEND_ID][0];

		FDCAN_Send(sendID, message, CAN_MESSAGE_SIZE);
	}
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

void floorIncrease(void)
{
	sendIDTmp++;
	receiveIDTmp++;
}
