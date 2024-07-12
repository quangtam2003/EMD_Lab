#include <lpc17xx.h>
#define PRESCALE (25000-1) //25000 PCLK clock cycles to increment TC by 1 

void delayMS(unsigned int milliseconds);
void initTimer0(void);

int main(void)
{
	//SystemInit(); //called by Startup Code before main(), hence no need to call again.
	LPC_GPIO2->FIODIR |= ((1<<2) |(1<<3) | (1<<4) | (1<<5)); //Configure P2.2 and p2.3 as output pins

	LPC_GPIO1->FIODIR &= ~((1<<26)|(1<<24)|(1<<23)|(1<<25)); //Configure P1.26 and P1.24 as input pin
	

	while(1)
	{
		if((LPC_GPIO1->FIOPIN & (1<<26))==0)
		{
			LPC_GPIO2->FIOPIN ^= (1<<2); //Toggle bit P2.2
			delayMS(500);
		}
		
		if((LPC_GPIO1->FIOPIN & (1<<24))==0)
		{
			LPC_GPIO2->FIOPIN ^= (1<<4); //Toggle bit P2.3
			delayMS(500);
		}
		
		if((LPC_GPIO1 -> FIOPIN & (1<<23)) == 0)
		{
			LPC_GPIO2 -> FIOPIN ^= (1<<3); //Toggle bit P2.4
			delayMS(500);
		}
		
		if((LPC_GPIO1 -> FIOPIN & (1<<25)) == 0)
		{
			LPC_GPIO2 -> FIOPIN ^= (1<<5); //Toggle bit P2.5
			delayMS(500);
		}
		
	}

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