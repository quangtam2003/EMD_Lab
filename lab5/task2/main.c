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


#define JOYSTICK_UP_MASK     (1<<23)  // Joystick Up button (P1.23)
#define JOYSTICK_DOWN_MASK   (1<<25)  // Joystick Down button (P1.25)
#define JOYSTICK_CENTER_MASK (1<<20)  // Joystick Center button (P1.20)

#define LCD_WIDTH  320
#define LCD_HEIGHT 240

#define MENU_ITEMS 4
#define ITEM_HEIGHT 40

// Global variables
volatile int current_item = 0;
volatile bool menu_updated = false;
char timeString[9];
int i;
typedef struct
{
	unsigned char Hour;
	unsigned char Minute;
	unsigned char Second;
} Time;
Time Now;



// Function prototypes
void GLCD_UpdateMenu(void);
void Main_menu_Joy_stick_scan(void);
void Time_update(void);
void TimeToString(char* string, Time* time);
void Time_display(void);
void menu_action_1(void);
void menu_action_2(void);
void menu_action_3(void);
void menu_action_4(void);


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
    {"ADC Threshold Set", &menu_action_4}
};


void menu_action_1(void);
void menu_action_2(void);
void menu_action_3(void);
void menu_action_4(void);

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
  
  if (TimerId) {
    osTimerStart (TimerId, 1000);
  }
  
  ADC_Initialize          ();           /* Initialize A/D Converter           */
  GLCD_Initialize         ();           /* Initialize Graphical LCD           */
  Joystick_Initialize     ();           /* Initialize joystick                */
  LED_Initialize          ();           /* Initialize LED                     */

  /* Prepare display for ADC, Buttons, Joystick */
  GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
  GLCD_SetForegroundColor (GLCD_COLOR_RED);
  GLCD_SetFont            (&GLCD_Font_16x24);
  GLCD_ClearScreen();
  GLCD_DrawString         (0, 0*24, "   Automatic Pump");
  GLCD_DrawString         (0, 1*24, "       Project");
  GLCD_SetBackgroundColor (GLCD_COLOR_WHITE);
  GLCD_SetForegroundColor (GLCD_COLOR_BLUE);
	GLCD_UpdateMenu();

 
  

  while (1) 
		{
			       // Update the menu if necessary
        if (menu_updated) 
				{
            GLCD_UpdateMenu();
            menu_updated = false;
        }
        Main_menu_Joy_stick_scan();
				Time_display();

    }
}
// Action for menu item 1
void menu_action_1(void) {
    // Action code for menu item 1
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 1*24, "A1");

}

// Action for menu item 2
void menu_action_2(void) {
    // Action code for menu item 2
		  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 1*24, "A2");
}

// Action for menu item 3
void menu_action_3(void) {
		GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 1*24, "A3");
 
}

// Action for menu item 4
void menu_action_4(void) {
    // Action code for menu item 4
		GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 1*24, "A4");
}

// Update the menu display
void GLCD_UpdateMenu(void) 
{

    for (i = 0; i < MENU_ITEMS; i++) 
	   {
        // Calculate the position of the menu item
        int x = 10;
        int y = 60 + (ITEM_HEIGHT * i);

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

void Main_menu_Joy_stick_scan(void)
{
	        // Joystick Up button pressed
        if (!(LPC_GPIO1->FIOPIN & JOYSTICK_UP_MASK)) {
            if (current_item > 0) {
                current_item--;
                menu_updated = true;
                while (!(LPC_GPIO1->FIOPIN & JOYSTICK_UP_MASK)); // Wait until the button is released
            }
        }

        // Joystick Down button pressed
        if (!(LPC_GPIO1->FIOPIN & JOYSTICK_DOWN_MASK)) {
            if (current_item < MENU_ITEMS - 1) {
                current_item++;
                menu_updated = true;
                while (!(LPC_GPIO1->FIOPIN & JOYSTICK_DOWN_MASK)); // Wait until the button is released
            }
        }

        // Joystick Center button pressed
        if (!(LPC_GPIO1->FIOPIN & JOYSTICK_CENTER_MASK)) {
            // Perform action based on the selected menu item
            menu[current_item].action();
            while (!(LPC_GPIO1->FIOPIN & JOYSTICK_CENTER_MASK)); // Wait until the button is released
        }
	
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

void Time_display(void)
{
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "Clock:");
   	TimeToString(timeString, &Now);
	  GLCD_DrawString(7*16, 9*24, timeString);
	
}

