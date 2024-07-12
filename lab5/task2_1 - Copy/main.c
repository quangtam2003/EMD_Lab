#include "LPC17xx.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "Board_LED.h"
#include "Board_Buttons.h"
#include "Board_Joystick.h"
#include "Board_ADC.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

#define NOT_SELECTED						0
#define SELECTED								1
#define TITLE										2
#define STATIC									3
#define NOT_SELECTED_BACK_COLOR GLCD_COLOR_WHITE      // 255, 255, 255
#define NOT_SELECTED_FORE_COLOR	0x0600 								//   0, 192,   0
#define SELECTED_BACK_COLOR 		0xEF5D 								// 232, 232, 232
#define SELECTED_FORE_COLOR 		GLCD_COLOR_BLUE								//  64, 192,  64
#define TITLE_BACK_COLOR 				GLCD_COLOR_LIGHT_GREY // 192, 192, 192
#define TITLE_FORE_COLOR 				GLCD_COLOR_WHITE			// 255, 255, 255
#define STATIC_BACK_COLOR 			GLCD_COLOR_WHITE      // 255, 255, 255
#define STATIC_FORE_COLOR 			0x11E2 								//  16,	 60,  16


#define  MASK_JOYSTICK_UP     (1<<23)  // Joystick Up button (P1.23)
#define  MASK_JOYSTICK_DOWN   (1<<25)  // Joystick Down button (P1.25)
#define  MASK_JOYSTICK_CENTER (1<<20)  // Joystick Center button (P1.20)
#define  MASK_JOYSTICK_LEFT   (1<<26)  // Joystick left button (P1.26)
#define  MASK_JOYSTICK_RIGHT  (1<<24)  // Joystick right button (P1.24)


#define LCD_WIDTH  320
#define LCD_HEIGHT 240

#define MENU_ITEMS 5
#define ITEM_HEIGHT 40



// Global variables
volatile int current_item = 0;
volatile bool menu_updated = false;
char timeString[9];
int i;
int programIndex=1;
int Time_Index=0;

int Current_joystickStatus=0;
int Old_joystickStatus=0;
int Working_Index;
typedef struct
{
	unsigned char Hour;
	unsigned char Minute;
	unsigned char Second;
} Time;
Time Now,Pump_time;

typedef struct
{
	unsigned int		ADCValue;
	unsigned int		ADCThreshold;
	unsigned char		PumpDuration;
	bool				IsRunning;
	unsigned char		RemainTime;
} AutomaticPump;

AutomaticPump Data_pump;


// Function prototypes
void GLCD_intro_text(void);
void GLCD_UpdateMenu(void);
static void Main_menu_Joy_stick_scan(int joystickStatus);
void Time_update(void);
void TimeToString(char* string, Time* time);
void Time_and_data_init(void);
void TimeSettingScreen(void);
static void DisplayStaticText(void);
static void DisplayTime(void);
void Increase(unsigned char *value, unsigned char maxValue);
void Descrease(unsigned char *value, unsigned char maxValue);
void SelectColor(int textType);
void Joy_stick_scan(void);
void TimeSettingScreenJoystickControl(int joystickStatus);

void PumpSettingScreen(void);
static void Pump_DisplayStaticText(void);
static void P_DisplayTime(void);
void P_Setting_JoystickControl(int joystickStatus);

int P_setting_Index=1;
int P_Time_Index;

static void Working_Screen(void);
static void Display_Static_Text_data(void);
static void Display_current_Time(void);
static void Display_Pump_time(void);
static void DisplayADCValue(void);
static void DisplayPumpStatus(void);

// action 4
int ADC_prog_Index;
static void  ADC_Threshold_Set(void);
static void ADC_DisplayStaticText(void);
static void DisplayADC_Threshold();
void ADC_Threshold_JoystickControl(int joystickStatus);

// action 3
int Pump_duration_prog_Index;
static void  Pump_Duration_Set(void);
static void Duration_DisplayStaticText(void);
static void Display_Pump_Duration();
void Pump_Duration_JoystickControl(int joystickStatus);



void Time_display(void);
void menu_action_1(void);
void menu_action_2(void);
void menu_action_3(void);
void menu_action_4(void);
void menu_action_5(void);


// Menu item structure
typedef struct {
    const char* label;
    void (*action)(void);
} MenuItem;

// Menu items
MenuItem menu[MENU_ITEMS] = {
    {"Clock Setting", &menu_action_1},
    {"Pump Time Setting", &menu_action_2},
    {"Duration Setting", &menu_action_3},
    {"ADC Threshold Set", &menu_action_4},
	{"Working Status", &menu_action_5}
};


/* Periodic timer definition */
void Timer_Callback (void const *arg);
osTimerDef (PeriodicTimer, Timer_Callback);
osTimerId TimerId;

 


/*-----------------------------------------------------------------------------
  Periodic timer callback
 *----------------------------------------------------------------------------*/
void Timer_Callback (void const *arg) 
{
    Time_update();
}



/*-----------------------------------------------------------------------------
  Blink LEDs
 *----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
 
      TimerId = osTimerCreate (osTimer(PeriodicTimer), osTimerPeriodic, NULL);
  
      if (TimerId) 
	  {
           osTimerStart (TimerId, 1000);
      }
  
      ADC_Initialize          ();           /* Initialize A/D Converter           */
      GLCD_Initialize         ();           /* Initialize Graphical LCD           */
      Joystick_Initialize     ();           /* Initialize joystick                */
      LED_Initialize          ();           /* Initialize LED                     */
	  Time_and_data_init();

       /* Prepare display for ADC, Buttons, Joystick */
       GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
       GLCD_SetForegroundColor (GLCD_COLOR_RED);
       GLCD_SetFont            (&GLCD_Font_16x24);
       GLCD_intro_text();
	   GLCD_UpdateMenu();
  while (1) 
		{
	        // Update the menu if necessary
             if (menu_updated) 
				{
                     GLCD_UpdateMenu();
                     menu_updated = false;
                }
				Joy_stick_scan();
                Main_menu_Joy_stick_scan(Current_joystickStatus);;
				//Time_display();

         }
}
// Action for menu item 1
void menu_action_1(void) {
    // Action code for menu item 1
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A1");
	  TimeSettingScreen();

}

// Action for menu item 2
void menu_action_2(void) {
    // Action code for menu item 2
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A2");
	  PumpSettingScreen();
	
}

// Action for menu item 3
void menu_action_3(void) {
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	//  GLCD_DrawString(0, 9*24, "A3");
	Pump_Duration_Set();
}

// Action for menu item 4
void menu_action_4(void) {
    // Action code for menu item 4
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  //GLCD_DrawString(0, 9*24, "A4");
	  ADC_Threshold_Set();
}

// Action for menu item 5
void menu_action_5(void) {
    // Action code for menu item 5
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A5");
	  Working_Screen();
}

void GLCD_intro_text(void)
{	
	GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_ClearScreen();
    GLCD_DrawString         (0, 0*24, "Auto Pump Menu");

}

// Update the menu display
void GLCD_UpdateMenu(void) 
{

    for (i = 0; i < MENU_ITEMS; i++) 
	   {
        // Calculate the position of the menu item
        int x = 10;
        int y = 30 + (ITEM_HEIGHT * i);

        // If the current item, draw a filled rectangle in a different color
        if (i == current_item) 
					{
            GLCD_DrawRectangle(x - 2, y - 2, LCD_WIDTH - 14, ITEM_HEIGHT -10);
            GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
            GLCD_DrawString(x, y, menu[i].label);
          } else 
					{
						GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
						GLCD_DrawRectangle(x - 2, y - 2, LCD_WIDTH - 14, ITEM_HEIGHT -10);
            GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
            GLCD_DrawString(x, y, menu[i].label);
          }
    }
}


void Joy_stick_scan(void)
{
	        // Joystick Up button pressed
        if (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_UP)) 
					{
           Current_joystickStatus=JOYSTICK_UP;
					
           while (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_UP)); // Wait until the button is released
          
          }

        // Joystick Down button pressed
        if (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_DOWN)) 
					{
					Current_joystickStatus=JOYSTICK_DOWN;
					
          while (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_DOWN)); // Wait until the button is released
          
          }

        // Joystick Center button pressed
        if (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_CENTER)) 
					{
          
					Current_joystickStatus=JOYSTICK_CENTER;
         
            while (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_CENTER)); // Wait until the button is released
          }
        // Joystick Left button pressed
        if (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_LEFT)) 
					{
           
					Current_joystickStatus=JOYSTICK_LEFT;
						
          
            while (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_LEFT)); // Wait until the button is released
          }
        // Joystick Right button pressed
        if (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_RIGHT)) 
					{
					  Current_joystickStatus=JOYSTICK_RIGHT;
					
            while (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_RIGHT)); // Wait until the button is released
          }				
	
}

static void Main_menu_Joy_stick_scan(int joystickStatus)
{
	        // Joystick Up button pressed
        if (joystickStatus==JOYSTICK_UP) {
            if (current_item > 0) {
                current_item--;
                menu_updated = true;
                Current_joystickStatus=0;
            }
        }
        // Joystick Down button pressed
        if (joystickStatus==JOYSTICK_DOWN) {
            if (current_item < MENU_ITEMS - 1) {
                current_item++;
				menu_updated = true;
				Current_joystickStatus=0;
            }
        }
        // Joystick Center button pressed
        if (joystickStatus==JOYSTICK_CENTER) 
		{
            // Perform action based on the selected menu item
			Current_joystickStatus=0;
            menu[current_item].action();
        }

}




void Time_and_data_init(void)
{
	Now.Second=0;
	Now.Minute=0;
	Now.Hour = 0;
	
	Pump_time.Second=1;
	Pump_time.Minute=1;
	Pump_time.Hour = 1;
	Data_pump.ADCValue=300;
	Data_pump.ADCThreshold=500;
	Data_pump.PumpDuration=30;
	Data_pump.IsRunning=false;
	Data_pump.RemainTime=30;
}
void Time_update(void)
{
	Now.Second++;
	if(Now.Second == 60)
	{
		Now.Second = 0;
		Now.Minute++;
		if(Now.Minute == 60)
		{ 
			Now.Minute = 0;
			Now.Hour++;
			if(Now.Hour == 24)
			{
				Now.Hour = 0;
			}
		}
	}
}

void TimeToString(char* string, Time* time)
{
	sprintf(string, "%02d:%02d:%02d", time->Hour, time->Minute, time->Second);
}


void TimeSettingScreen(void)
{
	programIndex=1;
	Time_Index=0;
	DisplayStaticText();
	while(programIndex==1)
	{
		DisplayTime();
		Joy_stick_scan();
		TimeSettingScreenJoystickControl(Current_joystickStatus);
	}
	menu_updated = true;
	GLCD_intro_text();
}

static void DisplayStaticText(void)
{
	GLCD_ClearScreen();
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "   Clock Setting:");
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 1*24, "Press: Up,Down     ");
	GLCD_DrawString(0, 2*24, "       Left , Right");
	GLCD_DrawString(0, 3*24, "       To Adjust");
  SelectColor(STATIC);
	GLCD_DrawString(0, 5*24, "Current Time: ");	
	GLCD_DrawString(8*16, 7*24, "h:");
	GLCD_DrawString(12*16, 7*24, "m:");
	GLCD_DrawString(16*16, 7*24, "s");
	
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
	
}

static void DisplayTime(void)
{
	char timeString[3];
	
	SelectColor(Time_Index==0);
	sprintf(timeString, "%02d", Now.Hour);
	GLCD_DrawString(6*16, 7*24, timeString);
	
	SelectColor(Time_Index==1);
	sprintf(timeString, "%02d", Now.Minute);
	GLCD_DrawString(10*16, 7*24, timeString);
	
	SelectColor(Time_Index==2);
	sprintf(timeString, "%02d", Now.Second);
	GLCD_DrawString(14*16, 7*24, timeString);
}


void TimeSettingScreenJoystickControl(int joystickStatus)
{
	switch(joystickStatus)
	{
		case JOYSTICK_UP:
			switch(Time_Index)
			{
				case 0: Increase(&Now.Hour, 23); break;
				case 1: Increase(&Now.Minute, 59); break;
				case 2: Increase(&Now.Second, 59); break;
				default: break;
			}
			Current_joystickStatus=0;
			break;
		case JOYSTICK_DOWN:
			switch(Time_Index)
			{
				case 0: Descrease(&Now.Hour, 23); break;
				case 1: Descrease(&Now.Minute, 59); break;
				case 2: Descrease(&Now.Second, 59); break;
				default: break;
			}
			Current_joystickStatus=0;
			break;
		case JOYSTICK_LEFT:
			if (Time_Index>0)
			{
				Time_Index--;
			}
			Current_joystickStatus=0;
			break;
		case JOYSTICK_RIGHT:
			if (Time_Index<2)
			{
				Time_Index++;
			}
			Current_joystickStatus=0;
			break;
			case JOYSTICK_CENTER:
		  programIndex=0;
			Current_joystickStatus=0;
			break;
		default:
			break;
	}
}

void Increase(unsigned char *value, unsigned char maxValue)
{
	if((*value)>=maxValue)
	{
		*value=0;
	}
	else
	{
		(*value)++;
	}
}

void Descrease(unsigned char *value, unsigned char maxValue)
{
	if((*value)<=0)
	{
		*value=maxValue;
	}
	else
	{
		(*value)--;
	}
}

void SelectColor(int textType)
{
	if (textType == STATIC)
	{
		GLCD_SetBackgroundColor(STATIC_BACK_COLOR);
		GLCD_SetForegroundColor(STATIC_FORE_COLOR);
	}
	else if (textType == TITLE)
	{
		GLCD_SetBackgroundColor(TITLE_BACK_COLOR);
		GLCD_SetForegroundColor(TITLE_FORE_COLOR);
	}
	else if (textType == SELECTED)
	{
		GLCD_SetBackgroundColor(SELECTED_BACK_COLOR);
		GLCD_SetForegroundColor(SELECTED_FORE_COLOR);
	}
	else
	{
		GLCD_SetBackgroundColor(NOT_SELECTED_BACK_COLOR);
		GLCD_SetForegroundColor(NOT_SELECTED_FORE_COLOR);
	}
}


void Working_Screen(void)
{	
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	Working_Index=1;
	GLCD_intro_text();
	Display_Static_Text_data();
	while(Working_Index==1)
	{
		Display_current_Time();
		Display_Pump_time();
		DisplayADCValue();
		DisplayPumpStatus();
		Joy_stick_scan();
		if(	Current_joystickStatus==JOYSTICK_CENTER)
			{
		    Working_Index=0;
			  Current_joystickStatus=0;
			}
	}
  menu_updated = true;
	GLCD_intro_text();
}

static void Display_Static_Text_data(void)
{
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 2*24, "Clock: ");
	GLCD_DrawString(0, 3*24, "Pump_Time:");
	GLCD_DrawString(0, 4*24, "ADC Value:");
	GLCD_DrawString(0, 5*24, "ADC Threshold:");
	
	GLCD_DrawString(0, 6*24, "Pump_Duration:");
	GLCD_DrawString(0, 7*24, "Pump_Remain: ");
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	
}

static void Display_current_Time(void)
{
	char timeString[9];
	TimeToString(timeString, &Now);
	GLCD_DrawString(10*16, 2*24, timeString);
}

static void Display_Pump_time(void)
{
	char timeString[9];
	TimeToString(timeString, &Pump_time);
	GLCD_DrawString(10*16, 3*24, timeString);
}

static void DisplayADCValue(void)
{
	char adcString[5];
	sprintf(adcString, "%04d", Data_pump.ADCValue);
	GLCD_DrawString(11*16, 4*24, adcString);
	sprintf(adcString, "%04d", Data_pump.ADCThreshold);
	GLCD_DrawString(14*16, 5*24, adcString);
	sprintf(adcString, "%04d", Data_pump.PumpDuration);
	GLCD_DrawString(14*16, 6*24, adcString);
	sprintf(adcString, "%04d", Data_pump.RemainTime);
	GLCD_DrawString(14*16, 7*24, adcString);
}

static void DisplayPumpStatus(void)
{
	SelectColor(STATIC);
	if(Data_pump.IsRunning)
	{
		GLCD_DrawString(0, 8*24, "Pump is running.    ");
	}
	else
	{
		GLCD_DrawString(0, 8*24, "Pump is not running.");
	}
}




void PumpSettingScreen(void)
{
	P_setting_Index=1;
	P_Time_Index=0;
	Pump_DisplayStaticText();
	while(P_setting_Index==1)
	{
		P_DisplayTime();
		Joy_stick_scan();
		P_Setting_JoystickControl(Current_joystickStatus);
	}
	menu_updated = true;
	GLCD_intro_text();
}

static void Pump_DisplayStaticText(void)
{
	GLCD_ClearScreen();
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "Pump Time Setting:");
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 1*24, "Press: Up,Down     ");
	GLCD_DrawString(0, 2*24, "       Left , Right");
	GLCD_DrawString(0, 3*24, "       To Adjust");
  SelectColor(STATIC);
	GLCD_DrawString(0, 5*24, "Pump Time: ");	
	GLCD_DrawString(8*16, 7*24, "h:");
	GLCD_DrawString(12*16, 7*24, "m:");
	GLCD_DrawString(16*16, 7*24, "s");
	
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
	
}

static void P_DisplayTime(void)
{
	char timeString[3];
	
	SelectColor(P_Time_Index==0);
	sprintf(timeString, "%02d", Pump_time.Hour);
	GLCD_DrawString(6*16, 7*24, timeString);
	
	SelectColor(P_Time_Index==1);
	sprintf(timeString, "%02d", Pump_time.Minute);
	GLCD_DrawString(10*16, 7*24, timeString);
	
	SelectColor(P_Time_Index==2);
	sprintf(timeString, "%02d", Pump_time.Second);
	GLCD_DrawString(14*16, 7*24, timeString);
}


void P_Setting_JoystickControl(int joystickStatus)
{
	switch(joystickStatus)
	{
		case JOYSTICK_UP:
			switch(P_Time_Index)
			{
				case 0: Increase(&Pump_time.Hour, 23); break;
				case 1: Increase(&Pump_time.Minute, 59); break;
				case 2: Increase(&Pump_time.Second, 59); break;
				default: break;
			}
			Current_joystickStatus=0;
			break;
		case JOYSTICK_DOWN:
			switch(P_Time_Index)
			{
				case 0: Descrease(&Pump_time.Hour, 23); break;
				case 1: Descrease(&Pump_time.Minute, 59); break;
				case 2: Descrease(&Pump_time.Second, 59); break;
				default: break;
			}
			Current_joystickStatus=0;
			break;
		case JOYSTICK_LEFT:
			if (P_Time_Index>0)
			{
				P_Time_Index--;
			}
			Current_joystickStatus=0;
			break;
		case JOYSTICK_RIGHT:
			if (P_Time_Index<2)
			{
				P_Time_Index++;
			}
			Current_joystickStatus=0;
			break;
			case JOYSTICK_CENTER:
		  P_setting_Index=0;
			Current_joystickStatus=0;
			break;
		default:
			break;
	}
}


static void  ADC_Threshold_Set(void)
{
	ADC_prog_Index=1;
	ADC_DisplayStaticText();
	while(ADC_prog_Index==1)
	{
		DisplayADC_Threshold();
		Joy_stick_scan();
		ADC_Threshold_JoystickControl(Current_joystickStatus);
	}
	menu_updated = true;
	GLCD_intro_text();
}

static void ADC_DisplayStaticText(void)
{
	GLCD_ClearScreen();
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "ADCThreshold Setting:");
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 1*24, "Press: Up,Down     ");
	GLCD_DrawString(0, 3*24, "       To Adjust");
	SelectColor(STATIC);
	GLCD_DrawString(0, 5*24, "ADC Threshold: ");	
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

static void DisplayADC_Threshold()
{
	char ADC_Threshold_String[3];
	
	sprintf(ADC_Threshold_String, "%02d", Data_pump.ADCThreshold);
	GLCD_DrawString(16*24, 5*24, "ADC Threshold: ");
	GLCD_DrawString(16*16, 7*24, ADC_Threshold_String);
}

void ADC_Threshold_JoystickControl(int joystickStatus)
{
	switch(joystickStatus)
	{
		case JOYSTICK_UP:
		if (Data_pump.ADCThreshold < 1024)
		{
			Data_pump.ADCThreshold ++;
		}
		

			Current_joystickStatus=0;
			break;
		case JOYSTICK_DOWN:
		if (Data_pump.ADCThreshold > 0)
		{
			Data_pump.ADCThreshold --;
		}


			Current_joystickStatus=0;
			break;

			case JOYSTICK_CENTER:
			ADC_prog_Index=0;
			Current_joystickStatus=0;
			break;
		default:
			break;
	}
}

// action 3


static void  Pump_Duration_Set(void)
{
	Pump_duration_prog_Index=1;
	Duration_DisplayStaticText();
	
	while(Pump_duration_prog_Index==1)
	{
		DisplayADC_Threshold();
		Joy_stick_scan();
		TimeSettingScreenJoystickControl(Current_joystickStatus);
	}
	menu_updated = true;
	GLCD_intro_text();
}

static void Duration_DisplayStaticText(void)
{
	GLCD_ClearScreen();
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "Pump Duration Set:");
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 1*24, "Press: Up,Down     ");
	GLCD_DrawString(0, 3*24, "       To Adjust");
	SelectColor(STATIC);
	GLCD_DrawString(0, 5*24, " Duration: ");	
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

static void Display_Pump_Duration()
{
	char Pump_Duration_String[3];
	
	sprintf(Pump_Duration_String, "%02d", Data_pump.PumpDuration);
	GLCD_DrawString(16*24, 5*24, "Pump Duration:");
	GLCD_DrawString(16*16, 7*24, Pump_Duration_String);
}

void Pump_Duration_JoystickControl(int joystickStatus)
{
	switch(joystickStatus)
	{
		case JOYSTICK_UP:
		if (Data_pump.PumpDuration < 250)
		{
			Data_pump.PumpDuration ++;
		}
		

			Current_joystickStatus=0;
			break;
		case JOYSTICK_DOWN:
		if (Data_pump.PumpDuration > 0)
		{
			Data_pump.PumpDuration --;
		}


			Current_joystickStatus=0;
			break;

			case JOYSTICK_CENTER:
			Pump_duration_prog_Index=0;
			Current_joystickStatus=0;
			break;
		default:
			break;
	}
}