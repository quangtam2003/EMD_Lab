#include <lpc17xx.h>
#define PRESCALE (25000-1)  // PCLK=1.000.000/1.000 = 1000Hz=> T_PCLK=1ms
void delayms(unsigned int milliseconds);
void Timer2_Init(void);
int main(void)
{
	Timer2_Init();
	LPC_GPIO2->FIODIR = (1<<4);
	while(1)
	{
		LPC_GPIO2->FIOSET = (1<<4);
		delayms(3000);
		LPC_GPIO2->FIOCLR = (1<<4);
		delayms(3000);
	}
	return 0;
}
 
void Timer2_Init(void)
{
	LPC_SC->PCONP |= (1<<22);       // turn on Timer2 module
	LPC_SC->PCLKSEL1 &=~(0x03<<13);  //0b010000; // systemcore clock/4= 100MHz/4 =25MHz
 
	LPC_TIM2->PR = PRESCALE;
	LPC_TIM2->TCR = 0x02;
}
 
void delayms(unsigned int milliseconds)
{
	LPC_TIM2->TCR = 0x02;   // reset counter b1 of TCR =1
	LPC_TIM2->TCR = 0x01;   // enable couter to count
 
	while(LPC_TIM2->TC < milliseconds);
 
	LPC_TIM2->TCR = 0x00;
}