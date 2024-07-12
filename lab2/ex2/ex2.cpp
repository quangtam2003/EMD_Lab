#include <lpc17xx.h>
#define PRESCALE (25000-1)  // PCLK=50.000.000/50.000 = 1000Hz=> T_PCLK=1ms
void delayms(unsigned int milliseconds);
void Timer1_Init(void);
int main(void)
{
	Timer1_Init();
	LPC_GPIO2->FIODIR = (1<<3);
	while(1)
	{
		LPC_GPIO2->FIOSET = (1<<3);
		delayms(1000);
		LPC_GPIO2->FIOCLR = (1<<3);
		delayms(1000);
	}
	return 0;
}
 
void Timer1_Init(void)
{
	LPC_SC->PCONP |= (1<<2);       // turn on Timer1 module
	LPC_SC->PCLKSEL0 &= ~(0x03<<5) ; //0b010000; // systemcore clock/4= 100MHz/4 =25MHz = 25 000 000 HZ
 
	LPC_TIM1->PR = PRESCALE;  // 25 000 000Hz / 25000 = 1000Hz => T = 1/f = 1/1000 = 1ms
	LPC_TIM1->TCR = 0x02;
}
 
void delayms(unsigned int milliseconds)
{
	LPC_TIM1->TCR = 0x02;   // reset counter b1 of TCR =1
	LPC_TIM1->TCR = 0x01;   // enable couter to count
 
	while(LPC_TIM1->TC < milliseconds);
 
	LPC_TIM1->TCR = 0x00;  // stop timer
}