#include <lpc17xx.h>

#define PRESCALE (25000-1) //25000 PCLK clock cycles to increment TC by 1 

void initTimer0();
unsigned int led_index=0,mode=0;


             

int main(void) 
{
	//SystemInit(); //called by Startup Code before main(), hence no need to call again.
	LPC_GPIO1->FIODIR |= (1<<28)| (1<<29)|(1<<31); 
	LPC_GPIO2->FIODIR |= (1<<2)|(1<<3)|(1<<4) |(1<<5)|(1<<6); 
	
	
	initTimer0();
	
	while(1)
	{
		
		}
	}
	//return 0; //normally this won't execute



void initTimer0(void)
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
	if(mode==0)
	{
		switch(led_index)
		{  
			case 0: LPC_GPIO1->FIOPIN |= (1<<28); //TURN ON LED0
				   break;
			case 1: LPC_GPIO1->FIOPIN |= (1<<29); //TURN ON LED0
				   break;	
			case 2: LPC_GPIO1->FIOPIN |= (1<<31); //TURN ON LED0
				   break;
			case 3: LPC_GPIO2->FIOPIN |= (1<<2); //TURN ON LED0
				   break;			   
			case 4: LPC_GPIO2->FIOPIN |= (1<<3); //TURN ON LED0
				   break;
			case 5: LPC_GPIO2->FIOPIN |= (1<<4); //TURN ON LED0
				   break;
			case 6: LPC_GPIO2->FIOPIN |= (1<<5); //TURN ON LED0
				   break;
			case 7: LPC_GPIO2->FIOPIN |= (1<<6); //TURN ON LED0
				   break;			   
			
		}
	}
	
	if(mode==1)
	{
		switch(led_index)
		{  
			case 0: LPC_GPIO1->FIOPIN &=~ (1<<28); //TURN OFF LED0
				   break;
			case 1: LPC_GPIO1->FIOPIN &=~ (1<<29); //TURN OFF LED0
				   break;	
			case 2: LPC_GPIO1->FIOPIN &=~ (1<<31); //TURN OFF LED0
				   break;
			case 3: LPC_GPIO2->FIOPIN &=~ (1<<2); //TURN OFF LED0
				   break;			   
			case 4: LPC_GPIO2->FIOPIN &=~ (1<<3); //TURN OFF LED0
				   break;
			case 5: LPC_GPIO2->FIOPIN &=~ (1<<4); //TURN OFF LED0
				   break;
			case 6: LPC_GPIO2->FIOPIN &=~ (1<<5); //TURN OFF LED0
				   break;
			case 7: LPC_GPIO2->FIOPIN &=~ (1<<6); //TURN OFF LED0
				   break;			   
			
		}
	}
	
	if(mode==2)
	{
		switch(led_index)
		{
			case 0:  LPC_GPIO1->FIOPIN |= (1<<28);
				break;
			case 1:  
				LPC_GPIO1->FIOPIN &=~ (1<<28);
				LPC_GPIO1->FIOPIN |= (1<<29);
				break;
			case 2: 
				LPC_GPIO1->FIOPIN &=~ (1<<29);
				LPC_GPIO1->FIOPIN |= (1<<31);
				break;
			case 3: 
				LPC_GPIO1->FIOPIN &=~ (1<<31);
				LPC_GPIO2->FIOPIN |= (1<<2);
				break;
			case 4: 
				LPC_GPIO2->FIOPIN &=~ (1<<2);
				LPC_GPIO2->FIOPIN |= (1<<3);
				break;
			case 5: 
				LPC_GPIO2->FIOPIN &=~ (1<<3);
				LPC_GPIO2->FIOPIN |= (1<<4);
				break;
			case 6: 
				LPC_GPIO2->FIOPIN &=~ (1<<4);
				LPC_GPIO2->FIOPIN |= (1<<5);
				break;
			case 7: 
				LPC_GPIO2->FIOPIN &=~ (1<<5);
				LPC_GPIO2->FIOPIN |= (1<<6);
				break;
			case 8: 
				LPC_GPIO2->FIOPIN &=~ (1<<6);
				break;
		}
	}
	led_index++;
	if(led_index>8)
	   {
		led_index=0;
		mode++;
		if(mode>3)
			mode=0;
	   }
}