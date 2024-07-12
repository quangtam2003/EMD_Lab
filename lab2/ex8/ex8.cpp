#include <lpc17xx.h>

#define PRESCALE (100000-1) //25000 PCLK clock cycles to increment TC by 1 

void initTimer3();

int main(void) 
{
	//SystemInit(); //called by Startup Code before main(), hence no need to call again.
	LPC_GPIO2->FIODIR |= (1<<5); //set P2.5 as output
	initTimer3();
	
	while(1)
	{
		//Idle loop
	}
	//return 0; //normally this won't execute
}


void initTimer3(void)
{
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	LPC_SC->PCONP |= (1<<23); //TIMER3
	LPC_SC->PCLKSEL1 &= ~(0x3<<15); //Set PCLK for timer = CCLK/4 = 100/4 (default)
	
	LPC_TIM3->CTCR = 0x0;
	LPC_TIM3->PR = PRESCALE; // 25 000 000 / 100 000 = 250Hz => T = 1/f = 1/250 = 4ms
	
	LPC_TIM3->MR0 = 4000; //Toggle Time in mS
	LPC_TIM3->MCR |= (1<<0) | (1<<1); // Interrupt & Reset on MR0 match
	LPC_TIM3->TCR |= (1<<1); //Reset Timer0

	NVIC_EnableIRQ(TIMER3_IRQn); //Enable timer interrupt
	
	LPC_TIM3->TCR = 0x01; //Enable timer
}

extern "C" void TIMER3_IRQHandler(void) //Use extern "C" so C++ can link it properly, for C it is not required
{
	LPC_TIM3->IR |= (1<<0); //Clear MR0 Interrupt flag
	LPC_GPIO2->FIOPIN ^= (1<<5); //Toggle LED
}