#include <stdbool.h>
#include "Time.h"

#ifndef PUMPSETTING_H
#define PUMPSETTING_H

#define PUMP_ADC_MODE	 1
#define PUMP_TIME_MODE 2

typedef struct
{
	unsigned char		PumpMode;
	unsigned int		ADCValue;
	unsigned int		ADCThreshold;
	Time       		  ActivateTime;
	unsigned char		PumpDuration;
	bool						IsRunning;
	unsigned char		RemainTime;
} AutomaticPump;

void TurnOnPump(void);
void TurnOffPump(void);
#endif
