#include "LPC17xx.h"                   // Device header
#include "Joystick.h"

/* Joystick pins:
   - center: P1_20 = GPIO1[20]
   - up:     P1_23 = GPIO1[23]
   - down:   P1_25 = GPIO1[25]
   - left:   P1_26 = GPIO1[26]
   - right:  P1_24 = GPIO1[24] */
	 
void Joystick_Initialize()
{
	LPC_SC->PCONP       |= (1 << 15);    // Enable power to GPIO & IOCON
	LPC_GPIO1->FIODIR &= ~0x07900000;		 // P1.26,25,24,23,20 are inputs
}

Joystick_Status_Enum Joystick_Get_Status()
{
	unsigned int Gpio1Status = LPC_GPIO1->FIOPIN;
	if(~Gpio1Status & (1<<20))
	{
		return JOYSTICK_CENTER;
	}
	else if(~Gpio1Status & (1<<23))
	{
		return JOYSTICK_UP;
	}
	else if(~Gpio1Status & (1<<25))
	{
		return JOYSTICK_DOWN;
	}
	else if(~Gpio1Status & (1<<26))
	{
		return JOYSTICK_LEFT;
	}
	else if(~Gpio1Status & (1<<24))
	{
		return JOYSTICK_RIGHT;
	}
	return UNDEFINED;
}
