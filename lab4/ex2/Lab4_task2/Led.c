#include "LPC17xx.h"                   // Device header
#include "Led.h"

const long led_Index[] = {1<<6, 1<<5, 1<<4, 1<< 3, 1<< 2, 1<< 31, 1<< 29, 1<< 28};

void Led_Initialize()
{
  LPC_SC->PCONP       |= (1 << 15);    // Enable power to GPIO & IOCON
  LPC_GPIO1->FIODIR   |= 0xB0000000;   // P1.28,29,31 are outputs
  LPC_GPIO2->FIODIR   |= 0x0000007C;   // P2.2,3,4,5,6 are outputs
}

void Led_On(unsigned int index)
{
  if(index < 5)
  {
    LPC_GPIO2->FIOSET =  led_Index[index];
  }
  else if(index < 8)
  {
    LPC_GPIO1->FIOSET =  led_Index[index];
  }
}

void Led_Off(unsigned int index)
{
  if(index < 5)
  {
    LPC_GPIO2->FIOCLR =  led_Index[index];
  }
  else if(index < 8)
  {
    LPC_GPIO1->FIOCLR =  led_Index[index];
  }
}

void Leds_Set_Value(char led_Value)
{
	int gpio2_Set_Value=0, gpio2_Clr_Value=0, gpio1_Set_Value=0, gpio1_Clr_Value=0;
	int i;
	for(i=0;i<5;i++)
	{
		if(led_Value&(1<<i))
		{
			gpio2_Set_Value|=led_Index[i];
		}
		else
		{
			gpio2_Clr_Value|=led_Index[i];
		}
	}
	for(i=5;i<8;i++)
	{
		if(led_Value&(1<<i))
		{
			gpio1_Set_Value|=led_Index[i];
		}
		else
		{
			gpio1_Clr_Value|=led_Index[i];
		}
	}
	LPC_GPIO2->FIOSET=gpio2_Set_Value;
	LPC_GPIO2->FIOCLR=gpio2_Clr_Value;
	LPC_GPIO1->FIOSET=gpio1_Set_Value;
	LPC_GPIO1->FIOCLR=gpio1_Clr_Value;
}
