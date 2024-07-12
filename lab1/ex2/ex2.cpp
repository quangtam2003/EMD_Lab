#include <lpc17xx.h>

int main(void)
{
	//SystemInit(); //called by Startup Code before main(), hence no need to call again.
	LPC_GPIO2->FIODIR |= (1<<2); //Configure P2.2 and p2.3 as output pins

	LPC_GPIO1->FIODIR &= ~(1<<26); //Configure P1.26 as input pin

	while(1)
	{
	if((LPC_GPIO1->FIOPIN & (1<<26))==0)
	{
		LPC_GPIO2->FIOPIN ^= (1<<2); //Turn all LED OFF
	}
	}

}