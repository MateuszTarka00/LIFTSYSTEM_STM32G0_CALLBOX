/*
 * flash.h
 *
 *  Created on: 23 paź 2025
 *      Author: mateo
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

typedef struct
{
	uint32_t floorNumberIdSend;
	uint8_t floorNumberIdReceive;
}Flash_floorNumberStruct;

extern Flash_floorNumberStruct flash_floorNumber;

void Flash_ErasePage(uint32_t pageIndex);
void Flash_WriteStruct(uint32_t pageIndex, const Flash_floorNumberStruct *data);
void Flash_ReadStruct(uint32_t pageIndex, Flash_floorNumberStruct *data);
uint32_t Flash_GetPageAddress(uint32_t pageIndex);
uint8_t checkStructEmpty(const Flash_floorNumberStruct *data);
bool saveValues(void);
void loadValues(uint32_t *idSend, uint8_t *idReceive);


#endif /* INC_FLASH_H_ */
