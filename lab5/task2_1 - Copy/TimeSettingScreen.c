#include <stdio.h>
#include "Board_Joystick.h"
#include "Board_GLCD.h"
#include "PumpSetting.h"
#include "Time.h"
#include "Common.h"

static void DisplayStaticText(void);
static void DisplayTime(void);
void TimeSettingScreenJoystickControl(int joystickStatus);

void DisplayTimeSettingScreen(void)
{
	ScreenInitialize("    Time Setting    ");
	DisplayStaticText();
	while(programIndex==1)
	{
		DisplayTime();
	}
}

static void DisplayStaticText(void)
{
	SelectColor(STATIC);
	GLCD_DrawString(0, 2*24, "Current Time: ");	
	GLCD_DrawString(8*16, 3*24, "h:");
	GLCD_DrawString(12*16, 3*24, "m:");
	GLCD_DrawString(16*16, 3*24, "s");
}

static void DisplayTime(void)
{
	char timeString[3];
	
	SelectColor(menuIndex==0);
	sprintf(timeString, "%02d", Now.Hour);
	GLCD_DrawString(6*16, 3*24, timeString);
	
	SelectColor(menuIndex==1);
	sprintf(timeString, "%02d", Now.Minute);
	GLCD_DrawString(10*16, 3*24, timeString);
	
	SelectColor(menuIndex==2);
	sprintf(timeString, "%02d", Now.Second);
	GLCD_DrawString(14*16, 3*24, timeString);
}

void TimeSettingScreenJoystickControl(int joystickStatus)
{
	switch(joystickStatus)
	{
		case JOYSTICK_UP:
			switch(menuIndex)
			{
				case 0: Increase(&Now.Hour, 23); break;
				case 1: Increase(&Now.Minute, 59); break;
				case 2: Increase(&Now.Second, 59); break;
				default: break;
			}
			break;
		case JOYSTICK_DOWN:
			switch(menuIndex)
			{
				case 0: Descrease(&Now.Hour, 23); break;
				case 1: Descrease(&Now.Minute, 59); break;
				case 2: Descrease(&Now.Second, 59); break;
				default: break;
			}
			break;
		case JOYSTICK_LEFT:
			if (menuIndex>0)
			{
				menuIndex--;
			}
			break;
		case JOYSTICK_RIGHT:
			if (menuIndex<2)
			{
				menuIndex++;
			}
			break;
		default:
			break;
	}
}
