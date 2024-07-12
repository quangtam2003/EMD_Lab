#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include <GLCD_Config.h>
#include <Board_GLCD.h>
#include <GLCD_Fonts.h>

#include <Driver_SPI.h>
#define PRESCALE (25000-1)  // PCLK=1.000.000/1.000 = 1000Hz=> T_PCLK=1ms
void delayms(unsigned int milliseconds);
void Timer1_Init(void);
char static myString[]={""};
int static x = 10;
int static y = 10;

int main( void ) {
	Timer1_Init();
	GLCD_Initialize()	;      
	GLCD_SetFont(&GLCD_Font_16x24)	;
	GLCD_SetBackgroundColor	(GLCD_COLOR_LIGHT_GREY);
	GLCD_SetBackgroundColor	(GLCD_COLOR_WHITE)	;
	
	GLCD_ClearScreen()	;
//	GLCD_DrawBargraph(100,405,50,50,100);
//	GLCD_DrawBargraph(160,405,50,50,100);
//	GLCD_DrawBargraph(100,150,50,50,20);
//	GLCD_DrawString	(30,460,myString);
	while ( 1 ) {
			GLCD_SetForegroundColor	(GLCD_COLOR_BLACK);
			GLCD_DrawRectangle((uint32_t)x,(uint32_t)y,100,100);
			delayms(100);
			GLCD_SetForegroundColor	(GLCD_COLOR_WHITE);
			GLCD_DrawRectangle((uint32_t)x,(uint32_t)y,100,100);
			if(x<=200 && y==10){
				x+=10;
			}
			
			else if(x>200 && y<=120){
				y+=10;
			}
			
			else if(x>10 && y>120){
				x-=10;
			}
			else if(x==10){
				y-=10;
			}
			GLCD_SetForegroundColor	(GLCD_COLOR_BLACK);
			GLCD_DrawRectangle((uint32_t)x,(uint32_t)y,100,100);
			delayms(100);
		
		
		
	}
}

void Timer1_Init(void)
{
	LPC_SC->PCONP |= (1<<2);       // turn on Timer3 module
	LPC_SC->PCLKSEL0 &= ~(0x3<<5) ; //0b010000; // systemcore clock/4= 100MHz/8 =25MHz
  LPC_TIM1->CTCR = 0x0;
	LPC_TIM1->PR = PRESCALE; // 25 000 000 / 25 000 = 1000  T = 1/f = =1ms
	LPC_TIM1->TCR = 0x02;
}
 
void delayms(unsigned int milliseconds)
{
	LPC_TIM1->TCR = 0x02;   // reset counter b1 of TCR =1
	LPC_TIM1->TCR = 0x01;   // enable couter to count
 
	while(LPC_TIM1->TC < milliseconds);
 
	LPC_TIM1->TCR = 0x00;
}