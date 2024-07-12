#include <lpc17xx.h>
#define PRESCALE (100000-1)  // PCLK=1.000.000/1.000 = 1000Hz=> T_PCLK=1ms
void delayms(unsigned int milliseconds);
void Timer3_Init(void);
int main(void)
{
	Timer3_Init();
	LPC_GPIO2->FIODIR = (1<<5);
	while(1)
	{
		LPC_GPIO2->FIOSET = (1<<5);
		delayms(4000);
		LPC_GPIO2->FIOCLR = (1<<5);
		delayms(4000);
	}
	return 0;
}
 
void Timer3_Init(void)
{
	LPC_SC->PCONP |= (1<<23);       // turn on Timer3 module
	LPC_SC->PCLKSEL1 &=~(0x03<<15);  //0b010000; // systemcore clock/4= 100MHz/4 =25MHz
 
	LPC_TIM3->CTCR = 0x0;
	LPC_TIM3->PR = PRESCALE; // 25 000 000Hz / 100 000 = 250Hz => T = 1/f = 1/250 = 4ms
	LPC_TIM3->TCR = 0x02;
}
 
void delayms(unsigned int milliseconds)
{
	LPC_TIM3->TCR = 0x02;   // reset counter b1 of TCR =1
	LPC_TIM3->TCR = 0x01;   // enable couter to count
 
	while(LPC_TIM3->TC < milliseconds);
 
	LPC_TIM3->TCR = 0x00;
}