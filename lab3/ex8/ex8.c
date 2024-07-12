#include "LPC17xx.h"
#define POWER_CONTROL (LPC_SC->PCONP)

#define PINCONSEL1 (LPC_PINCON->PINSEL1)
#define PINCONSEL2 (LPC_PINCON->PINSEL2)

#define IO1_DIR (LPC_GPIO1->FIODIR)
#define IO2_DIR (LPC_GPIO2->FIODIR)

#define IO1_SET (LPC_GPIO1->FIOSET)
#define IO2_SET (LPC_GPIO2->FIOSET)

#define IO1_CLR (LPC_GPIO1->FIOCLR)
#define IO2_CLR (LPC_GPIO2->FIOCLR)

#define IO1_PIN (LPC_GPIO1->FIOPIN)
#define IO2_PIN (LPC_GPIO2->FIOPIN)

#define LED1_BIT (1 << 6)
#define LED2_BIT (1 << 5)
#define LED3_BIT (1 << 4)
#define LED4_BIT (1 << 3)
#define LED5_BIT (1 << 2)
#define LED6_BIT (1 << 31)
#define LED7_BIT (1 << 29)
#define LED8_BIT (1 << 28)

#define VREF       3.3 //Reference Voltage at VREFP pin, given VREFN = 0V(GND)
#define ADC_CLK_EN (1<<12)
#define SEL_AD0_2  (1<<2) //Select Channel AD0.0 
#define CLKDIV     1 //ADC clock-divider (ADC_CLOCK=PCLK/CLKDIV+1) = 12.5Mhz      
                     //25Mhz PCLK
#define PWRUP      (1<<21) //setting it to 0 will power it down
#define START_CNV  (1<<24) //001 for starting the conversion immediately
#define ADC_DONE   (1U<<31) //define it as unsigned value Done Bit
#define ADCR_SETUP_SCM ((CLKDIV<<8) | PWRUP)


void leds_init(void)
{
	POWER_CONTROL |= (1 << 15);
	IO2_DIR = (LED1_BIT | LED2_BIT | LED3_BIT | LED4_BIT | LED5_BIT);
	IO1_DIR = (LED6_BIT | LED7_BIT | LED8_BIT);
}

void led_on(unsigned int index)
{
	switch(index)
	{
		case 1: 
			IO2_SET = LED1_BIT;
			break;
		case 2: 
			IO2_SET = LED2_BIT;
			break;
		case 3: 
			IO2_SET = LED3_BIT;
			break;
		case 4: 
			IO2_SET = LED4_BIT;
			break;
		case 5: 
			IO2_SET = LED5_BIT;
			break;
		case 6: 
			IO1_SET = LED6_BIT;
			break;
		case 7: 
			IO1_SET = LED7_BIT;
			break;
		case 8: 
			IO1_SET = LED8_BIT;
			break;
		default: 
			break;
	}
}

void led_off(unsigned int index)
{
	switch(index)
	{
		case 1: 
			IO2_CLR = LED1_BIT;
			break;
		case 2: 
			IO2_CLR = LED2_BIT;
			break;
		case 3: 
			IO2_CLR = LED3_BIT;
			break;
		case 4: 
			IO2_CLR = LED4_BIT;
			break;
		case 5: 
			IO2_CLR = LED5_BIT;
			break;
		case 6: 
			IO1_CLR = LED6_BIT;
			break;
		case 7: 
			IO1_CLR = LED7_BIT;
			break;
		case 8: 
			IO1_CLR = LED8_BIT;
			break;
		default: 
			break;
	}
}

void leds_set_value(char led_Value)
{
	int i;
	for(i = 0; i < 8; i++)
	{
		if(led_Value & (1<<i))
		{
			led_on(i+1);
		}
		else
		{
			led_off(i+1);
		}
	}
}

void ADC_Initialize(void)
{
	LPC_SC->PCONP |= ADC_CLK_EN; //Enable ADC clock
	LPC_ADC->ADCR =  ADCR_SETUP_SCM | SEL_AD0_2;
	LPC_PINCON->PINSEL1 |= (1<<18) ; //select AD0.2 for P0.25
	
}
void ADC_StartConversion(void)
{
   LPC_ADC->ADCR |= START_CNV; //Start new Conversion
}

int main()
{
	leds_init();
	ADC_Initialize();
	int ADC_Value;
	char led_value;
	while(1)
	{
		ADC_StartConversion();
		while((LPC_ADC->ADDR2 & ADC_DONE) == 0); //Wait untill conversion is     
                                                 //finished
		ADC_Value = (LPC_ADC->ADDR2>>8) & 0xFF; //12 bit Mask to extract                     
                                                //result
		led_value = (char) ADC_Value;
		leds_set_value(led_value);
	}
}