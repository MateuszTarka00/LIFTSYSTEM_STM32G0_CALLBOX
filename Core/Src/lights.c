/*
 * lights.c
 *
 *  Created on: 7 maj 2026
 *      Author: mateo
 */

#include "lights.h"

static SoftwareTimerHandler outputsTimers[2];

LightsStruct upLight = {LED_UP_GPIO_Port, LED_UP_Pin, &outputsTimers[0]};
LightsStruct downLight = {LED_DOWN_GPIO_Port, LED_DOWN_Pin, &outputsTimers[1]};

void toggleLights(void * param)
{
	LightsStruct *digitalOutput = (LightsStruct *)param;
	HAL_GPIO_TogglePin(digitalOutput->port, digitalOutput->pin);
}

void initTimers(void)
{
	initSoftwareTimer(upLight.timer, LIGHTS_BLINK_MS, toggleLights, true, &upLight);
	initSoftwareTimer(downLight.timer, LIGHTS_BLINK_MS, toggleLights, true, &downLight);
}

void turnOnOffLight(LightsStruct *light, bool onOff)
{
	stopSoftwareTimer(light->timer);
	HAL_GPIO_WritePin(light->port, light->pin, onOff);
}

void startLightBlink(LightsStruct *light)
{
	startSoftwareTimer(light->timer);
}
