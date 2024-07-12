#include <LPC17xx.h>
#include <PIN_LPC17xx.h>
#include <GPIO_LPC17xx.h>
#include <GLCD_Config.h>
#include <Board_GLCD.h>
#include <GLCD_Fonts.h>

#include <Driver_SPI.h>

#define PRESCALE (50000-1)
void delayms(unsigned int milliseconds);
void Timer0_Init(void);

#define Joystick_key_up 23
#define Joystick_key_down 25
#define Joystick_key_left 24
#define Joystick_key_right 26

int x=10;
int y=160;

char myString[]={""};

int main( void ) {
	LPC_GPIO1->FIODIR &= ~((1<<Joystick_key_up)|(1<<Joystick_key_left)|(1<<Joystick_key_down)|(1<<Joystick_key_right));
	Timer0_Init();
	GLCD_Initialize()	;      
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24)	;
	GLCD_SetBackgroundColor	(GLCD_COLOR_BLACK);
	GLCD_SetBackgroundColor	(GLCD_COLOR_WHITE);
	GLCD_DrawString	(x,y,myString);
	while ( 1 ) {
		GLCD_SetForegroundColor	(GLCD_COLOR_BLACK);
		GLCD_DrawString	(x,y,myString);
		delayms(100);
		GLCD_SetForegroundColor	(GLCD_COLOR_WHITE);
		GLCD_DrawString	(x,y,myString);
		
		if((LPC_GPIO1->FIOPIN & (1<<Joystick_key_up))==0)
			{
				delayms(20);
				x=x+10;
				y=y;
				GLCD_DrawString	(x,y,myString);
			}
		if((LPC_GPIO1->FIOPIN & (1<<Joystick_key_down))==0)
			{
				delayms(20);
				x=x-10;
				y=y;
				GLCD_DrawString	(x,y,myString);
			}
	
		if((LPC_GPIO1->FIOPIN & (1<<Joystick_key_left))==0)
			{
				delayms(20);
				y=y-10;
				x=x;
				GLCD_DrawString	(x,y,myString);			
			}
		if((LPC_GPIO1->FIOPIN & (1<<Joystick_key_right))==0)
			{
				delayms(20);
				y=y+10;
				x=x;
				GLCD_DrawString	(x,y,myString);		
			}
		GLCD_SetForegroundColor	(GLCD_COLOR_BLACK);
		GLCD_DrawString	(x,y,myString);
		delayms(100);
		
	}
}

void Timer0_Init(void)
{
	LPC_SC->PCONP |= (1<<23); // Turn on the power for timer3
    LPC_SC->PCLKSEL0 |= (1<<3); // PCLKSEL0[3:2]="10" mean system clock /2 
                                // 100Mhz/2 = 50MHz/50.000 => 1000HHz => T=1ms
	LPC_TIM0->CTCR = 0x0;
	LPC_TIM0->PR = PRESCALE;
 
	LPC_TIM0->TCR = 0x02;
}
 
void delayms(unsigned int milliseconds)
{
	LPC_TIM0->TCR = 0x02;
	LPC_TIM0->TCR = 0x01;
 
	while(LPC_TIM0->TC < milliseconds);
 
	LPC_TIM0->TCR = 0x00;
}