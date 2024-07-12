#include <lpc17xx.h>

#define PRESCALE (25000-1) //25000 PCLK clock cycles to increment TC by 1 

void initTimer1();

int main(void) 
{
	//SystemInit(); //called by Startup Code before main(), hence no need to call again.
	LPC_GPIO2->FIODIR |= (1<<3); //set P1.29 as output
	initTimer1();
	
	while(1)
	{
		//Idle loop
	}
	//return 0; //normally this won't execute
}


void initTimer1(void)
{
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	LPC_SC->PCONP |= (1<<2); //Power up TIM0. By default TIM0 and TIM1 are enabled.
	LPC_SC->PCLKSEL1 &= ~(0x3<<5); //Set PCLK for timer = CCLK/4 = 100/4 (default)
	
	LPC_TIM1->CTCR = 0x0;
	LPC_TIM1->PR = PRESCALE; //Increment LPC_TIM0->TC at every 24999+1 clock cycles
	//25000 clock cycles @25Mhz = 1 mS
	
	LPC_TIM1->MR0 = 2000; //Toggle Time in mS
	LPC_TIM1->MCR |= (1<<0) | (1<<1); // Interrupt & Reset on MR0 match
	LPC_TIM1->TCR |= (1<<1); //Reset Timer0

	NVIC_EnableIRQ(TIMER1_IRQn); //Enable timer interrupt
	
	LPC_TIM1->TCR = 0x01; //Enable timer
}

extern "C" void TIMER1_IRQHandler(void) //Use extern "C" so C++ can link it properly, for C it is not required
{
	LPC_TIM1->IR |= (1<<0); //Clear MR0 Interrupt flag
	LPC_GPIO2->FIOPIN ^= (1<<3); //Toggle LED
}