/*
 * lights.h
 *
 *  Created on: 7 maj 2026
 *      Author: mateo
 */

#ifndef INC_LIGHTS_H_
#define INC_LIGHTS_H_

#include "main.h"
#include "softwareTimer_ms.h"

#define LIGHTS_BLINK_MS 250

typedef struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
	SoftwareTimerHandler *timer;
}LightsStruct;

extern LightsStruct upLight;
extern LightsStruct downLight;

void toggleLights(void * param);
void initTimers(void);
void turnOnOffLight(LightsStruct *light, bool onOff);
void startLightBlink(LightsStruct *light);


#endif /* INC_LIGHTS_H_ */
