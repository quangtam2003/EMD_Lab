#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include <GLCD_Config.h>
#include <Board_GLCD.h>
#include <GLCD_Fonts.h>
#include <Driver_SPI.h>
#include <stdio.h>

#define PRESCALE (25000-1) //25000 PCLK clock cycles to increment TC by 1 

void initTimer0();

char myString[]={"hello, world"};

char ADC_String[20]=" ";

 
#define VREF       3.3 //Reference Voltage at VREFP pin, given VREFN = 0V(GND)
#define ADC_CLK_EN (1<<12)
#define SEL_AD0_2  (1<<2) //Select Channel AD0.0 
#define CLKDIV     1 //ADC clock-divider (ADC_CLOCK=PCLK/CLKDIV+1) = 12.5Mhz      
                     //25Mhz PCLK
#define PWRUP      (1<<21) //setting it to 0 will power it down
#define START_CNV  (1<<24) //001 for starting the conversion immediately
#define ADC_DONE   (1U<<31) //define it as unsigned value Done Bit
#define ADCR_SETUP_SCM ((CLKDIV<<8) | PWRUP)

int result = 0;
int bar_value;
float volts = 0;
void ADC_init(void)
{
	LPC_SC->PCONP |= ADC_CLK_EN; //Enable ADC clock
	LPC_ADC->ADCR =  ADCR_SETUP_SCM | SEL_AD0_2;
	LPC_PINCON->PINSEL1 |= (1<<18) ; //select AD0.2 for P0.25

}

void LCD_disp_init(void)
{
	GLCD_Initialize()	;      
	GLCD_SetFont(&GLCD_Font_16x24)	;
	GLCD_SetBackgroundColor	(GLCD_COLOR_GREEN)	;
	GLCD_SetBackgroundColor	(GLCD_COLOR_LIGHT_GREY);
	GLCD_ClearScreen()	;
}

void ADC_display(void)
{
	volts = (result*VREF)/4096.0; //Convert result to Voltage
	bar_value = volts*100/3.3;
	sprintf(ADC_String, "Volts: %f V", volts);
	GLCD_DrawString	(5,200,ADC_String);
	GLCD_DrawBargraph (100,150,50,50,bar_value);
}

void ADC_get_data(void)
{
    LPC_ADC->ADCR |= START_CNV; //Start new Conversion
	while((LPC_ADC->ADDR2 & ADC_DONE) == 0); //Wait untill conversion is     
                                             //finished
	result = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract                     
                                          //result
}

int main( void ) {
   initTimer0();
   ADC_init();
   LCD_disp_init();
	while ( 1 ) 
	{
		ADC_display();
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
	
	LPC_TIM0->MR0 = 2000; //Toggle Time in mS
	LPC_TIM0->MCR |= (1<<0) | (1<<1); // Interrupt & Reset on MR0 match
	LPC_TIM0->TCR |= (1<<1); //Reset Timer0

	NVIC_EnableIRQ(TIMER0_IRQn); //Enable timer interrupt
	
	LPC_TIM0->TCR = 0x01; //Enable timer
}
void TIMER0_IRQHandler(void) //Use extern "C" so C++ can link it properly, for C it is not required
{
	    LPC_TIM0->IR |= (1<<0); //Clear MR0 Interrupt flag
	    ADC_get_data();

	
}