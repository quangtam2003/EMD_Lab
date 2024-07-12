#include <lpc17xx.h>
#define PRESCALE (5000-1)
void delayms(unsigned int milliseconds);
void Timer0_Init(void);
int main(void)
{
	Timer0_Init();
	LPC_GPIO2->FIODIR = (1<<2);
	while(1)
	{
			LPC_GPIO2->FIOSET = (1<<2);
			delayms(1000);
			LPC_GPIO2->FIOCLR = (1<<2);
			delayms(1000);
	}
	return 0;
}
 
void Timer0_Init(void)
{
	LPC_SC->PCONP |= (1<<1);
	LPC_SC->PCLKSEL0 |= (0x02<<3); // systemcore clock/2 = 100MHz/2 =50MHz = 25 000 000 HZ
 
	LPC_TIM0->CTCR = 0x0;
	LPC_TIM0->PR = PRESCALE; // 50 000 000 / 5000 = 1000Hz
 
	LPC_TIM0->TCR = 0x02;
}
 
void delayms(unsigned int milliseconds)
{
	LPC_TIM0->TCR = 0x02;
	LPC_TIM0->TCR = 0x01;
 
	while(LPC_TIM0->TC < milliseconds);
 
	LPC_TIM0->TCR = 0x00;
}