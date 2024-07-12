#include "GLCD_Config.h"
#include "Common.h"

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

void ScreenInitialize(char* screenTitle)
{
	SelectColor(NOT_SELECTED);
	GLCD_ClearScreen();
	SelectColor(TITLE);
  GLCD_SetFont(&GLCD_Font_16x24);
  GLCD_DrawString(0, 0*24, "     HCMIU - EE     ");
  GLCD_DrawString(0, 1*24, screenTitle);
	menuIndex = 0;
	oldMenuIndex = -1;
}
