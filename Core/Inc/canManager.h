/*
 * canManager.h
 *
 *  Created on: Jan 19, 2026
 *      Author: mateo
 */

#ifndef INC_CANMANAGER_H_
#define INC_CANMANAGER_H_

#include "main.h"

typedef struct {
    uint32_t ident;  /*!< Standard identifier */
    uint8_t dlc;     /*!< Data length */
    uint8_t data[8]; /*!< Received data */
} CANrxMsg_t;

#endif /* INC_CANMANAGER_H_ */
