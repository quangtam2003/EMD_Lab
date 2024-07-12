#include "LPC17xx.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"

void TimeSettingScreen(void);
void PumpSettingScreen(void);
void Pump_Duration_Set(void);
void ADC_Threshold_Set(void);
void Working_Screen(void);
void Light_Sensor_Screen(void);
void Temp_Sensor_Screen(void);
void Moist_Sensor_Screen(void);

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
	  GLCD_DrawString(0, 9*24, "A3");
		Pump_Duration_Set();
}

// Action for menu item 4
void menu_action_4(void) {
    // Action code for menu item 4
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A4");
	  ADC_Threshold_Set();
}

// Action for menu item 5
void menu_action_5(void) {
    // Action code for menu item 5
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A5");  
		Working_Screen();
		
}

void menu_action_6(void) {
    // Action code for menu item 6
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A6");  
		Light_Sensor_Screen();
}

void menu_action_7(void) {
    // Action code for menu item 7
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A7");  
		Temp_Sensor_Screen();
		
}

void menu_action_8(void) {
    // Action code for menu item 8
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A8");  
		Moist_Sensor_Screen();
		
}

void menu_action_9(void) {
    // Action code for menu item 9
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A9");  
		Water_Sprinkler_Screen();
		
}

void menu_action_10(void) {
    // Action code for menu item 10
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A10");  
		Water_South_Screen();
		
}
void menu_action_11(void) {
    // Action code for menu item 11
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A11");  
		Water_North_Screen();
		
}
void menu_action_12(void) {
    // Action code for menu item 12
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A12");  
		Water_West_Screen();
		
}
void menu_action_13(void) {
    // Action code for menu item 13
	  GLCD_SetForegroundColor (GLCD_COLOR_BLACK);
	  GLCD_DrawString(0, 9*24, "A13");  
		Water_East_Screen();
		
}