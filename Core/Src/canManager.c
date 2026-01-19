/*
 * canManager.c
 *
 *  Created on: Jan 19, 2026
 *      Author: mateo
 */

#include "canManager.h"
#include "fdcan.h"

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo0ITs)
{
	static FDCAN_RxHeaderTypeDef rx_hdr;
	CANrxMsg_t rcvMsg;

	if (HAL_FDCAN_GetRxMessage(hfdcan, RxFifo0ITs, &rx_hdr, rcvMsg.data) != HAL_OK) {
		return;
	}

	if(rx_hdr.IdType !=	FDCAN_STANDARD_ID)
		return;
}

void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef* hfdcan, uint32_t RxFifo1ITs)
{
	static FDCAN_RxHeaderTypeDef rx_hdr;
	CANrxMsg_t rcvMsg;

	if (HAL_FDCAN_GetRxMessage(hfdcan, RxFifo1ITs, &rx_hdr, rcvMsg.data) != HAL_OK) {
		return;
	}

	if(rx_hdr.IdType !=	FDCAN_STANDARD_ID)
		return;
}
