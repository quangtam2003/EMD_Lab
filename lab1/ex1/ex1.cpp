#include <lpc17xx.h>

#define PRESCALE (25000-1) //25000 PCLK clock cycles to increment TC by 1 

void delayMS(unsigned int milliseconds);
void initTimer0(void);

int main(void)
{
	//SystemInit(); //called by Startup Code before main(), hence no need to call again.
	initTimer0(); //Initialize Timer0 
	LPC_GPIO2->FIODIR = (1<<2); //Configure P2.2 as output

	while(1)
	{
		LPC_GPIO2->FIOSET = (1<<2); //Turn ON LED P2.2
		delayMS(1000); //1 Second(s) Delay
		LPC_GPIO2->FIOCLR = (1<<2); //Turn LED OFF P2.2
		delayMS(1000); //1 Second(s) Delay
	}

	//return 0; //normally this wont execute ever
}

void initTimer0(void)
{
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	LPC_SC->PCONP |= (1<<1); //Power up TIM0. By default TIM0 and TIM1 are enabled.
	LPC_SC->PCLKSEL0 &= ~(0x3<<3); //Set PCLK for timer = CCLK/4 = 100/4 (default)

	LPC_TIM0->CTCR = 0x0;
	LPC_TIM0->PR = PRESCALE; //Increment LPC_TIM0->TC at every 24999+1 clock cycles
	//25000 clock cycles @25Mhz = 1 mS

	LPC_TIM0->TCR = 0x02; //Reset Timer
	}

void delayMS(unsigned int milliseconds) //Using Timer0
{
	LPC_TIM0->TCR = 0x02; //Reset Timer
	LPC_TIM0->TCR = 0x01; //Enable timer

	while(LPC_TIM0->TC < milliseconds); //wait until timer counter reaches the desired delay

	LPC_TIM0->TCR = 0x00; //Disable timer
}