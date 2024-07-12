#include <lpc17xx.h>

#define PRESCALE (25000-1) //25000 PCLK clock cycles to increment TC by 1 

void initTimer0();
void initTimer1();
void initTimer2();
void initTimer3();

int main(void) 
{
	//ex5
	LPC_GPIO2->FIODIR |= (1<<2); //set P2.2 as output
	initTimer0();
	
	//ex6
	LPC_GPIO2->FIODIR |= (1<<3); //set P2.3 as output
	initTimer1();
	
	//ex7
	LPC_GPIO2->FIODIR |= (1<<4); //set P2.4 as output
	initTimer2();
	
	//ex8
	LPC_GPIO2->FIODIR |= (1<<5); //set P2.5 as output
	initTimer3();
	
	
	while(1)
	{
		//Idle loop
	}
	//return 0; //normally this won't execute
}


void initTimer0(void) //ex5
{
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	LPC_SC->PCONP |= (1<<1); //Power up TIM0. By default TIM0 and TIM1 are enabled.
	LPC_SC->PCLKSEL0 &= ~(0x3<<3); //Set PCLK for timer = CCLK/4 = 100/4 (default)
	
	LPC_TIM0->CTCR = 0x0;
	LPC_TIM0->PR = PRESCALE; //Increment LPC_TIM0->TC at every 24999+1 clock cycles
	//25000 clock cycles @25Mhz = 1 mS
	
	LPC_TIM0->MR0 = 1000; //Toggle Time in mS
	LPC_TIM0->MCR |= (1<<0) | (1<<1); // Interrupt & Reset on MR0 match
	LPC_TIM0->TCR |= (1<<1); //Reset Timer0

	NVIC_EnableIRQ(TIMER0_IRQn); //Enable timer interrupt
	
	LPC_TIM0->TCR = 0x01; //Enable timer
}

extern "C" void TIMER0_IRQHandler(void) //Use extern "C" so C++ can link it properly, for C it is not required
{
	LPC_TIM0->IR |= (1<<0); //Clear MR0 Interrupt flag
	LPC_GPIO2->FIOPIN ^= (1<<2); //Toggle LED
}


void initTimer1(void) //ex6
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


void initTimer2(void) //ex7
{
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	LPC_SC->PCONP |= (1<<22); //TIMER2 (1<<22)
	LPC_SC->PCLKSEL1 &= ~(0x3<<13); //Set PCLK for timer = CCLK/4 = 100/4 (default) // TIMER 2 PCLKSEL1
	
	LPC_TIM2->CTCR = 0x0;
	LPC_TIM2->PR = PRESCALE; //Increment LPC_TIM0->TC at every 24999+1 clock cycles	
	//25000 clock cycles @25Mhz = 1 mS
	
	LPC_TIM2->MR0 = 4000; //Toggle Time in mS
	LPC_TIM2->MCR |= (1<<0) | (1<<1); // Interrupt & Reset on MR0 match
	LPC_TIM2->TCR |= (1<<1); //Reset Timer0

	NVIC_EnableIRQ(TIMER2_IRQn); //Enable timer interrupt
	
	LPC_TIM2->TCR = 0x01; //Enable timer
}	

extern "C" void TIMER2_IRQHandler(void) //Use extern "C" so C++ can link it properly, for C it is not required
{
	LPC_TIM2->IR |= (1<<0); //Clear MR0 Interrupt flag
	LPC_GPIO2->FIOPIN ^= (1<<4); //Toggle LED
}

void initTimer3(void) //ex8
{
	/*Assuming that PLL0 has been setup with CCLK = 100Mhz and PCLK = 25Mhz.*/
	LPC_SC->PCONP |= (1<<23); //TIMER3
	LPC_SC->PCLKSEL1 &= ~(0x3<<15); //Set PCLK for timer = CCLK/4 = 100/4 (default)
	
	LPC_TIM3->CTCR = 0x0;
	LPC_TIM3->PR = PRESCALE; // 25 000 000 / 100 000 = 250Hz => T = 1/f = 1/250 = 4ms
	
	LPC_TIM3->MR0 = 8000; //Toggle Time in mS
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
