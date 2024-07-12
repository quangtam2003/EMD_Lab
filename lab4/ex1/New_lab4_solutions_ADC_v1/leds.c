#include "LPC17xx.h"
#include "leds.h"
#include "os_tick.h"

volatile uint32_t msTicks;
    //! The interrupt handler for the SysTick module
    
void SysTick_Handler(void) 
{
      msTicks++;
}

void my_Delay (uint32_t dlyTicks) {
      uint32_t curTicks;
      curTicks = msTicks;
      while ((msTicks - curTicks) < dlyTicks) { __NOP(); }
}
void leds_init(void)
{
	POWER_CONTROL |= (1 << 15);
	IO2_DIR = (LED1_BIT | LED2_BIT | LED3_BIT | LED4_BIT | LED5_BIT);
	IO1_DIR = (LED6_BIT | LED7_BIT | LED8_BIT);
}

void led_on(unsigned int index)
{
	switch(index)
	{
		case 1: 
			IO2_SET = LED1_BIT;
			break;
		case 2: 
			IO2_SET = LED2_BIT;
			break;
		case 3: 
			IO2_SET = LED3_BIT;
			break;
		case 4: 
			IO2_SET = LED4_BIT;
			break;
		case 5: 
			IO2_SET = LED5_BIT;
			break;
		case 6: 
			IO1_SET = LED6_BIT;
			break;
		case 7: 
			IO1_SET = LED7_BIT;
			break;
		case 8: 
			IO1_SET = LED8_BIT;
			break;
		default: 
			break;
	}
}

void led_off(unsigned int index)
{
	switch(index)
	{
		case 1: 
			IO2_CLR = LED1_BIT;
			break;
		case 2: 
			IO2_CLR = LED2_BIT;
			break;
		case 3: 
			IO2_CLR = LED3_BIT;
			break;
		case 4: 
			IO2_CLR = LED4_BIT;
			break;
		case 5: 
			IO2_CLR = LED5_BIT;
			break;
		case 6: 
			IO1_CLR = LED6_BIT;
			break;
		case 7: 
			IO1_CLR = LED7_BIT;
			break;
		case 8: 
			IO1_CLR = LED8_BIT;
			break;
		default: 
			break;
	}
}

void Led_Style_OnOffEachLed(void)
{
  int i;
  for(i=1; i<=8; i++)
  {
    led_on(i);                        // Turn on Led i
    my_Delay(2000);                    // Delay 1000ms
    led_off(i);                       // Turn off Led i
  }
}

void Led_Stye_LedChasing(void)
{
	int i;
	for (i=1; i<=8; i++)
	{
		led_on(i);
		my_Delay(2000);
	}
	for (i=8; i>=1; i++)
	{
		led_off(i);
		my_Delay(2000);
	}
}

void Led_Style_3LedChasing(void)
{
	int i;
	for (i=1; i<=9; i++)
	{
		led_off(i-1);
		led_off(i-2);
		led_off(i-3);
		led_on(i);
		led_on(i+1);
		led_on(i+2);
		my_Delay(1000);
	}
}

void leds_set_value(char led_Value)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		if(led_Value & (1<<i))
		{
			led_on(i+1);
		}
		else
		{
			led_off(i+1);
		}
	}
}