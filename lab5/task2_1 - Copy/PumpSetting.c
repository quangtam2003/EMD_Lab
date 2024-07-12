#include "PumpSetting.h"
#include "Board_LED.h"

void TurnOnPump(void)
{
	LED_On(7);
}

void TurnOffPump(void)
{
	LED_Off(7);
}
