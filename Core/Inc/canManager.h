/*
 * canManager.h
 *
 *  Created on: Jan 19, 2026
 *      Author: mateo
 */

#ifndef INC_CANMANAGER_H_
#define INC_CANMANAGER_H_

#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"

#define CAN_MESSAGE_SIZE	4

extern volatile uint32_t sendID;
extern volatile uint8_t receiveID;

typedef struct
{
   uint32_t id;
   uint8_t len;
   uint8_t data[8];
} CAN_Message_t;

extern QueueHandle_t canRxQueue;

void CAN_InitRTOS(void);
void processMessage(CAN_Message_t *msg);
void floorIncrease(void);
void tranciverFunction(void);

#endif /* INC_CANMANAGER_H_ */
