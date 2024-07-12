#include "LPC17xx.h"
#include "cmsis_os.h"
#include <stdio.h>
#include "Board_LED.h"
#include "Board_Buttons.h"
#include "Board_Joystick.h"
//#include "Board_ADC.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "ADC.h"
#include "Driver_SPI.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

#define NOT_SELECTED						0
#define SELECTED								1
#define TITLE										2
#define STATIC									3
#define NOT_SELECTED_BACK_COLOR GLCD_COLOR_WHITE      // 255, 255, 255
#define NOT_SELECTED_FORE_COLOR	0x0600 								//   0, 192,   0
#define SELECTED_BACK_COLOR 		0xEF5D 								// 232, 232, 232
#define SELECTED_FORE_COLOR 		GLCD_COLOR_BLUE				//  64, 192,  64
#define TITLE_BACK_COLOR 				GLCD_COLOR_LIGHT_GREY // 192, 192, 192
#define TITLE_FORE_COLOR 				GLCD_COLOR_WHITE			// 255, 255, 255
#define STATIC_BACK_COLOR 			GLCD_COLOR_WHITE      // 255, 255, 255
#define STATIC_FORE_COLOR 			0x11E2 								//  16,	 60,  16

#define PRESCALE (25000-1) //25000 PCLK clock cycles to increment TC by 1 
#define VREF       3.3 //Reference Voltage at VREFP pin, given VREFN = 0V(GND)
#define ADC_CLK_EN (1<<12)
#define SEL_AD0_2  (1<<2) //Select Channel AD0.2
#define CLKDIV     1 //ADC clock-divider (ADC_CLOCK=PCLK/CLKDIV+1) = 12.5Mhz      
                     //25Mhz PCLK
#define PWRUP      (1<<21) //setting it to 0 will power it down
#define START_CNV  (1<<24) //001 for starting the conversion immediately
#define ADC_DONE   (1U<<31) //define it as unsigned value Done Bit
#define ADCR_SETUP_SCM ((CLKDIV<<8) | PWRUP)

#define  MASK_JOYSTICK_UP     (1<<23)  // Joystick Up button (P1.23)
#define  MASK_JOYSTICK_DOWN   (1<<25)  // Joystick Down button (P1.25)
#define  MASK_JOYSTICK_CENTER (1<<20)  // Joystick Center button (P1.20)
#define  MASK_JOYSTICK_LEFT   (1<<26)  // Joystick left button (P1.26)
#define  MASK_JOYSTICK_RIGHT  (1<<24)  // Joystick right button (P1.24)

#define LCD_WIDTH  320
#define LCD_HEIGHT 240

#define MENU_ITEMS 5
#define MENU_ITEMS_SENSOR 5
#define ITEM_HEIGHT 40

// Global variables
volatile int current_item = 0;
volatile bool menu_updated = false;
volatile bool menu_status = false;
char timeString[9];
char adcString[5];
char lightString[5];
char tempString[5];
char moistString[5];
char waterString[5];
char pressureString[5];

int i;
int j;
int programIndex=1;
int Time_Index=0;
int result = 0;
int decimal;
float volts = 0;

int Current_joystickStatus=0;
int Old_joystickStatus=0;
int Working_Index;
int Light_prog_Index;
int Temp_prog_Index;
int Moist_prog_Index;
int Water_prog_Index;
int Pressure_prog_Index;

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
	unsigned int		PumpDuration;
	bool				IsRunning;
	unsigned char		RemainTime;
} AutomaticPump;
AutomaticPump Data_pump;

typedef struct
{
	unsigned int Parameter;
	bool detected;
} Light;
Light light;

typedef struct
{
	unsigned int Parameter;
	bool detected;
} Temp;
Temp temperature;

typedef struct
{
	unsigned int Parameter;
	bool detected;
} Moisture;
Moisture moist;

typedef struct
{
	unsigned int Parameter;
	bool detected;
} Water;
Water water;

typedef struct
{
	unsigned int Parameter;
	bool detected;
} Pressure;
Pressure pressure;

// Function prototypes
void GLCD_intro_text(void);
void GLCD_UpdateMenu(void);

void Increase(unsigned char *value, unsigned char maxValue);
void Decrease(unsigned char *value, unsigned char maxValue);

char ADC_String[20]=" ";

// main menu functions
void SelectColor(int textType);
void Joy_stick_scan(void);
 void Main_menu_Joy_stick_scan(int joystickStatus);
void TimeSettingScreenJoystickControl(int joystickStatus);

// action 1 functions
void Time_update(void);
void TimeToString(char* string, Time* time);
void Time_and_data_init(void);
void TimeSettingScreen(void);
void checkRemainTime(unsigned char);
 void DisplayStaticText(void);
 void DisplayTime(void);
void Time_display(void);

// action 2 functions
void PumpSettingScreen(void);
 void Pump_DisplayStaticText(void);
 void P_DisplayTime(void);
void P_Setting_JoystickControl(int joystickStatus);

int P_setting_Index=1;
int P_Time_Index;

// action 3 functions
int Pump_duration_prog_Index;
void Pump_Duration_Set(void);
 void Duration_DisplayStaticText(void);
void Display_Pump_Duration();
void Pump_Duration_JoystickControl(int joystickStatus);

// action 4 functions
int ADC_prog_Index;
void ADC_Threshold_Set(void);
 void ADC_DisplayStaticText(void);
void DisplayADC_Threshold();
void ADC_Threshold_JoystickControl(int joystickStatus);

// action 5 functions
void Working_Screen(void);
 void Display_Static_Text_data(void);
 void Display_current_Time(void);
 void Display_Pump_time(void);
 void DisplayADCValue(void);
 void DisplayPumpStatus(void);

// action 6 functions
void Light_Sensor_Screen(void);
void DisplayLightStatus(void);
void Display_Light_Text_data(void);
void light_on(int gpio, int pin);
void light_off(void);

// action 7 functions
void Temp_Sensor_Screen(void);
void DisplayTempStatus(void);
void Display_Temp_Text_data(void);

// action 8 functions
void Moist_Sensor_Screen(void);
void DisplayMoistStatus(void);
void Display_Moist_Text_data(void);

// action 9 functions
void Water_Sprinkler_Screen(void);
void DisplayWaterStatus(void);
void Display_Water_Text_data(void);

//action 10 funtions
void Pressure_Screen(void);
void DisplayPressureStatus(void);
void Display_Pressure_Text_Data(void);

// menu action
void menu_action_1(void);
void menu_action_2(void);
void menu_action_3(void);
void menu_action_4(void);
void menu_action_5(void);
void menu_action_6(void);
void menu_action_7(void);
void menu_action_8(void);
void menu_action_9(void);
void menu_action_10(void);

// Menu item structure
typedef struct {
    const char* label;
    void (*action)(void);
} MenuItem;

// Menu items for Pump
MenuItem menuPump[MENU_ITEMS] = {
    {"Clock Setting", 		&menu_action_1},
    {"Pump Time Setting", &menu_action_2},
    {"Duration Setting", 	&menu_action_3},
    {"ADC Threshold Set", &menu_action_4},
		{"Working Status", 		&menu_action_5}
};

// Menu items for Sensor
MenuItem menuSensor[MENU_ITEMS_SENSOR] = {
    {"Light Sensor", 			&menu_action_6},
    {"Temp Sensor", 			&menu_action_7},
    {"Moisture Sensor", 	&menu_action_8},
		{"Water Sprinkler",   &menu_action_9},
		{"Pressure Sensors",  &menu_action_10}
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

// Initial LED
void Init_LED()
{
	LPC_GPIO1->FIODIR |= (1<<28)| (1<<29)|(1<<31); 
	LPC_GPIO2->FIODIR |= (1<<2)|(1<<3)|(1<<4) |(1<<5)|(1<<6); 
}
/*-----------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
 
      TimerId = osTimerCreate (osTimer(PeriodicTimer), osTimerPeriodic, NULL);
			Init_LED();
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
			 
			/* ADC Set up*/
			LPC_SC->PCONP |= ADC_CLK_EN; //Enable ADC clock
			LPC_ADC->ADCR =  ADCR_SETUP_SCM | SEL_AD0_2;
			LPC_PINCON->PINSEL1 |= (1<<18) ; //select AD0.2 for P0.25
			
			while (1) 
			{
	        // Update the menu if necessary
          if (menu_updated) 
					{
						GLCD_UpdateMenu();
            menu_updated = false;
					}
					Joy_stick_scan();
					Main_menu_Joy_stick_scan(Current_joystickStatus);
      }
}

void GLCD_intro_text(void)
{	
	GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_ClearScreen();
  GLCD_DrawString(0, 0*24, "Auto Pump Menu");
}
void GLCD_outro_text(void)
{	
	GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_ClearScreen();
  GLCD_DrawString(0, 0*24, "Sensor Menu");
}
// Update the menu display
void GLCD_UpdateMenu(void) 
{
	if(menu_status==false)
	{
    for (i = 0; i < MENU_ITEMS; i++) 
	  {
        // Calculate the position of the menu item
        int x = 10;
        int y = 30 + (ITEM_HEIGHT * i);
			
        // If at the current item, draw a filled rectangle in a different color
        if (i == current_item) 
				{
					GLCD_DrawRectangle(x - 2, y - 2, LCD_WIDTH - 14, ITEM_HEIGHT -10);
          GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
          GLCD_DrawString(x, y, menuPump[i].label);
        } 
				else 
				{
					GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
					GLCD_DrawRectangle(x - 2, y - 2, LCD_WIDTH - 14, ITEM_HEIGHT -10);
          GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
          GLCD_DrawString(x, y, menuPump[i].label);
        }
    }
	}
	else
	{
		for (j = 0; j < MENU_ITEMS_SENSOR; j++)
	  {
        // Calculate the position of the menu item
        int a = 10;
        int b = 30 + (ITEM_HEIGHT * j);
			
        // If at the current item, draw a filled rectangle in a different color
        if (j == current_item) 
				{
					GLCD_DrawRectangle(a - 2, b - 2, LCD_WIDTH - 14, ITEM_HEIGHT -10);
          GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
          GLCD_DrawString(a, b, menuSensor[j].label);
        } 
				else 
				{
					GLCD_SetForegroundColor (GLCD_COLOR_WHITE);
					GLCD_DrawRectangle(a - 2, b - 2, LCD_WIDTH - 14, ITEM_HEIGHT -10);
          GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
          GLCD_DrawString(a, b, menuSensor[j].label);
        }
    }
	}
}


void Joy_stick_scan(void)
{
		// Joystick Up button pressed
    if (!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_UP)) 
		{
				Current_joystickStatus=JOYSTICK_UP;	
        while(!(LPC_GPIO1->FIOPIN & MASK_JOYSTICK_UP)); // Wait until the button is released
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

 void Main_menu_Joy_stick_scan(int joystickStatus)
{
	      // Joystick Up button pressed
        if (joystickStatus==JOYSTICK_UP) 
				{
					if(menu_status==false)
					{
            if (current_item > 0) 
						{
                current_item--;
                menu_updated = true;
                Current_joystickStatus=0;
            }
						else
						{
								current_item=0;
                menu_updated = true;
                Current_joystickStatus=0;
						}
					}
					else
					{
						if (current_item > 0)
						{
								current_item--;
                menu_updated = true;
                Current_joystickStatus=0;
						}
						else
						{
								GLCD_ClearScreen();
								GLCD_intro_text();
								current_item = MENU_ITEMS;
								menu_status=false;
						}	
					}
        }
				
        // Joystick Down button pressed
        if (joystickStatus==JOYSTICK_DOWN) 
				{
            if (menu_status==false)
						{
							if(current_item < MENU_ITEMS - 1)
							{
                current_item++;
								menu_updated = true;
								Current_joystickStatus=0;
							}
							else
							{
								GLCD_ClearScreen();
								GLCD_outro_text();
								current_item=-1;
								menu_status=true;
							}
            }
						else
						{
							if(current_item < MENU_ITEMS_SENSOR - 1)
							{
								current_item++;
								menu_updated = true;
								Current_joystickStatus=0;		
							}
							else
							{
								current_item=MENU_ITEMS_SENSOR - 1;
								menu_updated = true;
								Current_joystickStatus=0;
							}
            }
        }
				
        // Joystick Center button pressed
        if (joystickStatus==JOYSTICK_CENTER) 
				{
					if(menu_status==false)
					{
            // Perform action based on the selected menu item
						Current_joystickStatus=0;
            menuPump[current_item].action();
					}
					else
					{
            // Perform action based on the selected menu item
						Current_joystickStatus=0;
            menuSensor[current_item].action();
					}
        }
}



// Time Settings - menu action 1
void Time_and_data_init(void)
{
	Now.Second=0;
	Now.Minute=0;
	Now.Hour = 0;
	
	Pump_time.Second=0;
	Pump_time.Minute=2;
	Pump_time.Hour = 0;
	Data_pump.ADCThreshold=200;
	Data_pump.PumpDuration=30;
	Data_pump.IsRunning=false;
	Data_pump.RemainTime=30;
	light.detected=false;
	light.Parameter=0;
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
 void Display_current_Time(void)
{
	char timeString[9];
	TimeToString(timeString, &Now);
	GLCD_DrawString(10*16, 2*24, timeString);
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

 void DisplayStaticText(void)
{
	GLCD_ClearScreen();
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "   Clock Setting:");
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 1*24, "Press: Up, Down     ");
	GLCD_DrawString(0, 2*24, "       Left, Right");
	GLCD_DrawString(0, 3*24, "       To Adjust");
  SelectColor(STATIC);
	GLCD_DrawString(0, 5*24, "Current Time: ");	
	GLCD_DrawString(8*16, 7*24, "h:");
	GLCD_DrawString(12*16, 7*24, "m:");
	GLCD_DrawString(16*16, 7*24, "s");
	
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

 void DisplayTime(void)
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
				case 0: Decrease(&Now.Hour, 23); break;
				case 1: Decrease(&Now.Minute, 59); break;
				case 2: Decrease(&Now.Second, 59); break;
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

// Select Color for Text
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



// Pump Time Setting - menu action 2
 void Display_Pump_time(void)
{
	char timeString[9];
	TimeToString(timeString, &Pump_time);
	GLCD_DrawString(10*16, 3*24, timeString);
}
 void DisplayPumpStatus(void)
{
	SelectColor(STATIC);
	if((Now.Hour==Pump_time.Hour)&&(Now.Minute==Pump_time.Minute)&&(Now.Second==Pump_time.Second))
	{
		Data_pump.IsRunning=true;
	}
//	else
//	{
//		if(Data_pump.ADCValue<Data_pump.ADCThreshold)
//		{
//			Data_pump.IsRunning=true;
//		}
//		else
//		{
//			Data_pump.IsRunning=false;
//		}
//	}
	if(Data_pump.IsRunning)
	{
			GLCD_DrawString(0, 8*24, "Pump is running now.");
			LPC_GPIO2->FIOSET = (1<<2); //Turn ON LED P2.2
			checkRemainTime(Data_pump.RemainTime);
	}
	else
	{
			GLCD_DrawString(0, 8*24, "Pump is not running.");
			Data_pump.RemainTime=Data_pump.PumpDuration;
			LPC_GPIO2->FIOCLR = (1<<2); //Turn OFF LED P2.2
	}
}
void checkRemainTime(unsigned char t)
{
	if(t==0)
	{
		if(Data_pump.ADCValue>Data_pump.ADCThreshold)
		{
			Data_pump.IsRunning=false;
		}
		else
		{
			Data_pump.RemainTime=Data_pump.PumpDuration;
		}
	}
	else
	{
			Data_pump.RemainTime--;
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

 void Pump_DisplayStaticText(void)
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
 void P_DisplayTime(void)
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
				case 0: Decrease(&Pump_time.Hour, 23); break;
				case 1: Decrease(&Pump_time.Minute, 59); break;
				case 2: Decrease(&Pump_time.Second, 59); break;
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



// Duration Setting - menu action 3
 void  Pump_Duration_Set(void)
{
	Pump_duration_prog_Index=1;
	Duration_DisplayStaticText();
	
	while(Pump_duration_prog_Index==1)
	{
		Display_Pump_Duration();
		Joy_stick_scan();
		Pump_Duration_JoystickControl(Current_joystickStatus);
	}
	menu_updated = true;
	GLCD_intro_text();
}

 void Duration_DisplayStaticText(void)
{
	GLCD_ClearScreen();
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "Pump Duration Set:");
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 1*24, "Press: Up, Down");
	GLCD_DrawString(0, 3*24, "       To Adjust");
	SelectColor(STATIC);
	GLCD_DrawString(0, 5*24, "Duration: ");	
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

 void Display_Pump_Duration()
{
	char Pump_Duration_String[3];
	
	sprintf(Pump_Duration_String, "%02d", Data_pump.PumpDuration);
	GLCD_DrawString(12*16, 5*24, Pump_Duration_String);
}

void Pump_Duration_JoystickControl(int joystickStatus)
{
	switch(joystickStatus)
	{
		case JOYSTICK_UP:
			if (Data_pump.PumpDuration < 250)
			{
				Data_pump.PumpDuration++;
			}
			Current_joystickStatus=0;
			break;
			
		case JOYSTICK_DOWN:
			if (Data_pump.PumpDuration > 0)
			{
				Data_pump.PumpDuration--;
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



// ADC Threshold Setting - menu action 4
 void  ADC_Threshold_Set(void)
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

 void ADC_DisplayStaticText(void)
{
	GLCD_ClearScreen();
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "ADCThreshold Setting:");
	GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 1*24, "Press: Up, Down");
	GLCD_DrawString(0, 3*24, "       To Adjust");
	SelectColor(STATIC);
	GLCD_DrawString(0, 5*24, "ADC Threshold: ");	
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

 void DisplayADC_Threshold()
{
	char ADC_Threshold_String[3];
	sprintf(ADC_Threshold_String, "%02d", Data_pump.ADCThreshold);
	GLCD_DrawString(14*16, 5*24, ADC_Threshold_String);
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



// Working Screen - menu action 5
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
		if(Current_joystickStatus==JOYSTICK_CENTER)
			{
		    Working_Index=0;
			  Current_joystickStatus=0;
			}
		else
			{
				DisplayADCValue();
			}
	}
  menu_updated = true;
	GLCD_intro_text();
}
 void Display_Static_Text_data(void)
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
 void DisplayADCValue(void)
{
	LPC_ADC->ADCR |= START_CNV; //Start new Conversion
	while((LPC_ADC->ADDR2 & ADC_DONE) == 0); //Wait untill conversion is     
                                           //finished
	Data_pump.ADCValue = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract                     
																										//result
	sprintf(adcString, "%04d", Data_pump.ADCValue);
	GLCD_DrawString(11*16, 4*24, adcString);
	sprintf(adcString, "%04d", Data_pump.ADCThreshold);
	GLCD_DrawString(14*16, 5*24, adcString);
	sprintf(adcString, "%04d", Data_pump.PumpDuration);
	GLCD_DrawString(14*16, 6*24, adcString);
	sprintf(adcString, "%04d", Data_pump.RemainTime);
	GLCD_DrawString(14*16, 7*24, adcString);
}


// Light Sensor - menu action 6
 void Light_Sensor_Screen(void)
{
	Light_prog_Index=1;
	Display_Light_Text_data();
	while(Light_prog_Index==1)
	{
		DisplayLightStatus();
		Joy_stick_scan();
		if(Current_joystickStatus==JOYSTICK_CENTER)
			{
		    Light_prog_Index=0;
			  Current_joystickStatus=0;
			}
		else
		{
				GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
				GLCD_DrawString(5*16, 5*24, lightString);
		}
	}
	menu_updated = true;
	GLCD_outro_text();
}

void Display_Light_Text_data(void)
{
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_SetForegroundColor(GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "Light Sensor");
	GLCD_SetForegroundColor(GLCD_COLOR_PURPLE);
	GLCD_DrawString(0, 3*24, "Light Intensity: ");
	SelectColor(STATIC);
	GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
	GLCD_SetForegroundColor (GLCD_COLOR_RED);
}
 void DisplayLightStatus(void)
{
	LPC_ADC->ADCR |= START_CNV; //Start new Conversion
	while((LPC_ADC->ADDR2 & ADC_DONE) == 0); //Wait untill conversion is     
                                           //finished
	light.Parameter = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract                     
																										//result
	sprintf(lightString, "%04d", light.Parameter);
	if(light.Parameter<=100)
	{
		GLCD_DrawString(0, 7*24, "Completely dark.");
		light_off();
		light_on(1,28);
	}
	else if((light.Parameter<=1000) && (light.Parameter>100))
	{
		GLCD_DrawString(0, 7*24, "Light is weak.  ");
		light_off();
		light_on(1,29);
	}
	else if((light.Parameter<=3000) && (light.Parameter>1000))
	{
		GLCD_DrawString(0, 7*24, "Light is normal.");
		light_off();
		light_on(1,31);
	}
	else
	{
		GLCD_DrawString(0, 7*24, "Light is bright.");
		light_off();
		light_on(2,2);
	}
}


// Temp Sensor - menu action 7
 void Temp_Sensor_Screen(void)
{
	Temp_prog_Index=1;
	Display_Temp_Text_data();
	while(Temp_prog_Index==1)
	{
		DisplayTempStatus();
		Joy_stick_scan();
		if(Current_joystickStatus==JOYSTICK_CENTER)
			{
		    Temp_prog_Index=0;
			  Current_joystickStatus=0;
			}
		else
		{
				GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
				GLCD_DrawString	(5*16, 5*24,ADC_String);
		}
	}
	menu_updated = true;
	GLCD_outro_text();
}
void Display_Temp_Text_data()
{
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_ClearScreen();
	GLCD_SetForegroundColor(GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "Temperature Sensor");
	GLCD_SetForegroundColor(GLCD_COLOR_PURPLE);
	GLCD_DrawString(0, 3*24, "Temperature: ");
	GLCD_SetForegroundColor(GLCD_COLOR_DARK_GREEN);
	GLCD_DrawString(9*16, 5*24," *C");
	SelectColor(STATIC);
	GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

 void DisplayTempStatus(void)
{
	LPC_ADC->ADCR |= START_CNV; //Start new Conversion
	while((LPC_ADC->ADDR2 & ADC_DONE) == 0); //Wait untill conversion is     
                                           //finished
	temperature.Parameter = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract                     
																										//result
	result = temperature.Parameter/100;
	decimal = temperature.Parameter%100;
	sprintf(ADC_String, "%02d.%02d", result, decimal);
	GLCD_DrawString	(5*16, 5*24,ADC_String);
	if(temperature.Parameter<=1000)
	{
			GLCD_DrawString(0, 7*24, "Temp is cold.  ");
			light_off();
			light_on(2,3);
	}
	else if ((temperature.Parameter<=3000) && (temperature.Parameter>1000))
	{
			GLCD_DrawString(0, 7*24, "Temp is normal.");
			light_off();
			light_on(2,4);
	}
	else
	{
			GLCD_DrawString(0, 7*24, "Temp is hot.   ");
			light_off();
			light_on(2,5);
	}
}


// Moist Sensor - menu action 8
 void Moist_Sensor_Screen(void)
{
	Moist_prog_Index=1;
	Display_Moist_Text_data();
	while(Moist_prog_Index==1)
	{
		DisplayMoistStatus();
		Joy_stick_scan();
		if(Current_joystickStatus==JOYSTICK_CENTER)
			{
		    Moist_prog_Index=0;
			  Current_joystickStatus=0;
			}
		else
		{
				GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
				GLCD_DrawString(5*16, 5*24, moistString);
		}
	}
	menu_updated = true;
	GLCD_outro_text();
}

void Display_Moist_Text_data(void)
{
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_SetForegroundColor(GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "Moisture Sensor");
	GLCD_SetForegroundColor(GLCD_COLOR_PURPLE);
	GLCD_DrawString(0, 3*24, "Soil Moisture: ");
	SelectColor(STATIC);
	GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

 void DisplayMoistStatus(void)
{
	LPC_ADC->ADCR |= START_CNV; //Start new Conversion
	while((LPC_ADC->ADDR2 & ADC_DONE) == 0); //Wait untill conversion is     
                                           //finished
	moist.Parameter = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract                     
																										//result
	sprintf(moistString, "%04d", moist.Parameter);
	if(moist.Parameter<=2000)
	{
			GLCD_DrawString(0, 6*24, "Soil moisture");
			GLCD_DrawString(0, 7*24, "is very dry.      ");
			light_off();
	}
	else
	{
			GLCD_DrawString(0, 6*24, "Soil moisture");
			GLCD_DrawString(0, 7*24, "is saturated.");
			light_off();
			light_on(2,5);
	}
}

// Water Sprinkler - menu action 9
void Water_Sprinkler_Screen(void)
{
	Water_prog_Index=1;
	Display_Water_Text_data();
	while(Water_prog_Index==1)
	{
		DisplayWaterStatus();
		Joy_stick_scan();
		if(Current_joystickStatus==JOYSTICK_CENTER)
			{
		    Water_prog_Index=0;
			  Current_joystickStatus=0;
			}
		else
		{
				GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
				GLCD_DrawString(8*16, 3*24, waterString);
		}
	}
	menu_updated = true;
	GLCD_outro_text();

}

void Display_Water_Text_data(void)
{
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_SetForegroundColor(GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "Water Sprinkler");
	GLCD_SetForegroundColor(GLCD_COLOR_PURPLE);
	GLCD_DrawString(0, 3*24, "Status: ");
	SelectColor(STATIC);
	GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

void DisplayWaterStatus(void)
{
	LPC_ADC->ADCR |= START_CNV; //Start new Conversion
	while((LPC_ADC->ADDR2 & ADC_DONE) == 0); //Wait untill conversion is     
                                           //finished
	water.Parameter = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract                     
																										//result
	sprintf(waterString, "%04d", water.Parameter);
	if(water.Parameter<=2000)
	{
			GLCD_DrawString(0, 6*24, "Not Working      ");
			GLCD_DrawString(0, 7*24, "Need to be fixed.");
			light_off();
	}
	else
	{
			GLCD_DrawString(0, 6*24, "Working properly");
			GLCD_DrawString(0, 7*24, "                     ");
			light_off();
			light_on(2,5);
	}
}

// Pressure Sensors
void Pressure_Screen(void)
{
	Pressure_prog_Index=1;
	Display_Pressure_Text_Data();
	while(Pressure_prog_Index==1)
	{
		DisplayPressureStatus();
		Joy_stick_scan();
		if(Current_joystickStatus==JOYSTICK_CENTER)
			{
		    Pressure_prog_Index=0;
			  Current_joystickStatus=0;
			}
		else
		{
				GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
				GLCD_DrawString(8*16, 3*24, pressureString);
		}
	}
	menu_updated = true;
	GLCD_outro_text();

}

void Display_Pressure_Text_Data(void)
{
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_SetForegroundColor(GLCD_COLOR_RED);
	GLCD_DrawString(0, 0*24, "Pressure Sensor");
	GLCD_SetForegroundColor(GLCD_COLOR_PURPLE);
	GLCD_DrawString(0, 3*24, "Status: ");
	SelectColor(STATIC);
	GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
	GLCD_DrawString(0, 9*24, "Press ENTER Return");
}

void DisplayPressureStatus(void)
{
	LPC_ADC->ADCR |= START_CNV; //Start new Conversion
	while((LPC_ADC->ADDR2 & ADC_DONE) == 0); //Wait untill conversion is     
                                           //finished
	pressure.Parameter = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract                     
																										//result
	sprintf(pressureString, "%04d", pressure.Parameter);
	if(pressure.Parameter<=2000)
	{
			GLCD_DrawString(0, 6*24, "Low Pressure.     ");
			GLCD_DrawString(0, 7*24, "Alert!!!          ");
			light_off();
			light_on(2,6);
			light_on(2,5);
	}
	else if( pressure.Parameter > 2000 && pressure.Parameter <=3000)
	{
			GLCD_DrawString(0, 6*24, "Normal Pressure.    ");
			GLCD_DrawString(0, 7*24, "                    ");
			light_off();
			light_on(2,6);
			light_on(2,5);
			light_on(2,4);
	}
	
	else 
	{
			GLCD_DrawString(0, 6*24, "High Pressure.    ");
			GLCD_DrawString(0, 7*24, "Alert!!!          ");
			light_off();
			light_on(2,6);
			light_on(2,5);
			light_on(2,4);
			light_on(2,3);
	
	}
}
