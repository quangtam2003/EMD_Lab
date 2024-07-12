#include <stdio.h>
#include "Board_Joystick.h"
#include "Board_GLCD.h"
#include "PumpSetting.h"
#include "Time.h"
#include "Common.h"

const char* menu1String="Time Settings       ";
const char* menu2String="Pump Settings       ";

static void DisplayStaticText(void);
static void DisplayTime(void);
static void DisplayADCValue(void);
static void DisplayPumpStatus(void);
static void DisplayMenuItems(void);

void DisplayMenuScreen(void)
{	
	ScreenInitialize("Embedded System Lab ");
	DisplayStaticText();
	while(programIndex==0)
	{
		DisplayTime();
		DisplayADCValue();
		DisplayPumpStatus();
		DisplayMenuItems();
	}
}

static void DisplayStaticText(void)
{
	SelectColor(STATIC);
	GLCD_DrawString(0, 3*24, "                    ");
	GLCD_DrawString(0, 7*24, "Time: ");
	GLCD_DrawString(0, 8*24, "ADC Value: ");
	SelectColor(NOT_SELECTED);
	GLCD_DrawString(0, 4*24, menu2String);
}

static void DisplayTime(void)
{
	char timeString[9];
	SelectColor(STATIC);
	TimeToString(timeString, &Now);
	GLCD_DrawString(7*16, 7*24, timeString);
}

static void DisplayADCValue(void)
{
	char adcString[5];
	SelectColor(STATIC);
	sprintf(adcString, "%04d", Pump.ADCValue);
	GLCD_DrawString(11*16, 8*24, adcString);
}

static void DisplayPumpStatus(void)
{
	SelectColor(STATIC);
	if(Pump.IsRunning)
	{
		GLCD_DrawString(0, 9*24, "Pump is running.    ");
	}
	else
	{
		GLCD_DrawString(0, 9*24, "Pump is not running.");
	}
}

static void DisplayMenuItems(void)
{
	if(menuIndex != oldMenuIndex)
	{
		if(oldMenuIndex>=0)
		{
			SelectColor(NOT_SELECTED);
			switch(oldMenuIndex)
			{
				case 0: GLCD_DrawString(0, 2*24, menu1String); break;
				case 1: GLCD_DrawString(0, 4*24, menu2String); break;
			}
		}
		oldMenuIndex = menuIndex;
	
		SelectColor(SELECTED);
		switch(menuIndex)
		{
				case 0: GLCD_DrawString(0, 2*24, menu1String); break;
				case 1: GLCD_DrawString(0, 4*24, menu2String); break;
		}
	}
}

void MenuScreenJoystickControl(int joystickStatus)
{
	switch(joystickStatus)
	{
		case JOYSTICK_DOWN:
			if(menuIndex < 1)
			{
				menuIndex++;
			}
			break;
		case JOYSTICK_UP:
			if(menuIndex > 0)
			{
				menuIndex--;
			}
			break;
		case JOYSTICK_CENTER:
			programIndex = menuIndex + 1;
			break;
	}
}
