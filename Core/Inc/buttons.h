/*
 * buttons.h
 *
 *  Created on: 6 maj 2026
 *      Author: mateo
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include "main.h"
#include "FreeRTOS.h"

extern bool upRequest;
extern bool downRequest;

#define DEBOUNCE_TIME_MS			 	20

typedef struct
{
    TickType_t lastTick;
    uint8_t pendingPress;
    uint8_t pendingRelease;
} InputDebounce_t;

void buttonsSubTask(void);

#endif /* INC_BUTTONS_H_ */
