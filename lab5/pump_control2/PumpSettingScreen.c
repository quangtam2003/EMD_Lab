#include <stdio.h>
#include "Board_Joystick.h"
#include "Board_GLCD.h"
#include "PumpSetting.h"
#include "Time.h"
#include "Common.h"

static void DisplayStaticText(void);
static void DisplayTime(void);
static void DisplayPumpDuration(void);
static void DisplayADCPumpMode(void);
static void DisplayTimePumpMode(void);
void PumpSettingScreenJoystickControl(int joystickStatus);

void DisplayPumpSettingScreen(void)
{
	ScreenInitialize("    Pump Setting    ");
	DisplayStaticText();
	while(programIndex==2)
	{
		DisplayTime();
		DisplayPumpDuration();
		DisplayADCPumpMode();
		DisplayTimePumpMode();
	}
}

static void DisplayStaticText(void)
{
	SelectColor(STATIC);
	GLCD_DrawString(0, 2*24, "Time:");
	GLCD_DrawString(0, 3*24, "Duration:");
	GLCD_DrawString(13*16, 3*24, "seconds");
	GLCD_DrawString(0, 5*24, "-----PUMP MODES-----");
	GLCD_DrawString(0, 6*24, "ADC Value:");
	GLCD_DrawString(0, 8*24, "Preset Time:");
	GLCD_DrawString(11*16, 9*24, "h:");
	GLCD_DrawString(15*16, 9*24, "m:");
	GLCD_DrawString(19*16, 9*24, "s");
}

static void DisplayTime(void)
{
	char timeString[7];
	SelectColor(STATIC);
	TimeToString(timeString, &Now);
	GLCD_DrawString(10*16, 2*24, timeString);
}

static void DisplayPumpDuration(void)
{
	char pumpDurationString[4];
	SelectColor(menuIndex==0);
	sprintf(pumpDurationString, "%02d", Pump.PumpDuration);
	GLCD_DrawString(10*16, 3*24, pumpDurationString);
}

static void DisplayADCPumpMode(void)
{
	char ADCMinString[5];
	SelectColor(menuIndex==1);
	if (Pump.PumpMode & PUMP_ADC_MODE)
	{
		GLCD_DrawString(12*16, 6*24, "ON ");
	}
	else
	{
		GLCD_DrawString(12*16, 6*24, "OFF");
	}
	SelectColor(menuIndex==2);
	sprintf(ADCMinString, "%04d", Pump.ADCThreshold);
	GLCD_DrawString(9*16, 7*24, ADCMinString);
}

static void DisplayTimePumpMode(void)
{
	char timeString[3];
	
	SelectColor(menuIndex==3);
	if (Pump.PumpMode & PUMP_TIME_MODE)
	{
		GLCD_DrawString(12*16, 8*24, "ON ");
	}
	else
	{
		GLCD_DrawString(12*16, 8*24, "OFF");
	}
	
	SelectColor(menuIndex==4);
	sprintf(timeString, "%02d", Pump.ActivateTime.Hour);
	GLCD_DrawString(9*16, 9*24, timeString);
	SelectColor(menuIndex==5);
	sprintf(timeString, "%02d", Pump.ActivateTime.Minute);
	GLCD_DrawString(13*16, 9*24, timeString);
	SelectColor(menuIndex==6);
	sprintf(timeString, "%02d", Pump.ActivateTime.Second);
	GLCD_DrawString(17*16, 9*24, timeString);
}

void PumpSettingScreenJoystickControl(int joystickStatus)
{
	switch(joystickStatus)
	{
		case JOYSTICK_UP:
			switch(menuIndex)
			{
				case 0: Increase(&Pump.PumpDuration, 99); break;
				case 1: Pump.PumpMode |= PUMP_ADC_MODE; break;
				case 2:
					if(Pump.ADCThreshold < 4095-100)
					{
						Pump.ADCThreshold += 100;
					}
					break;
				case 3: Pump.PumpMode |= PUMP_TIME_MODE; break;
				case 4: Increase(&Pump.ActivateTime.Hour, 23); break;
				case 5: Increase(&Pump.ActivateTime.Minute, 59); break;
				case 6: Increase(&Pump.ActivateTime.Second, 59); break;
				default: break;
			}
			break;
		case JOYSTICK_DOWN:
			switch(menuIndex)
			{
				case 0: Descrease(&Pump.PumpDuration, 99); break;
				case 1: Pump.PumpMode &= ~PUMP_ADC_MODE; break;
				case 2:
					if(Pump.ADCThreshold > 100)
					{
						Pump.ADCThreshold -= 100;
					}
				break;
				case 3: Pump.PumpMode &= ~PUMP_TIME_MODE; break;
				case 4: Descrease(&(Pump.ActivateTime.Hour), 23); break;
				case 5: Descrease(&(Pump.ActivateTime.Minute), 59); break;
				case 6: Descrease(&(Pump.ActivateTime.Second), 59); break;
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
			if (menuIndex<6)
			{
				menuIndex++;
			}
			break;
		case JOYSTICK_CENTER:
			if (menuIndex==1)
			{
				Pump.PumpMode=0;
			}
			else if (menuIndex==3)
			{
				Pump.PumpMode=1;
			}
		default:
			break;
	}
}
