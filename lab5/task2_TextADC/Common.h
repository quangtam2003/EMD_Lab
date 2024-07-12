#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "Time.h"
#include "PumpSetting.h"

#define NOT_SELECTED						0
#define SELECTED								1
#define TITLE										2
#define STATIC									3
#define NOT_SELECTED_BACK_COLOR GLCD_COLOR_WHITE      // 255, 255, 255
#define NOT_SELECTED_FORE_COLOR	0x0600 								//   0, 192,   0
#define SELECTED_BACK_COLOR 		0xEF5D 								// 232, 232, 232
#define SELECTED_FORE_COLOR 		0x4608 								//  64, 192,  64
#define TITLE_BACK_COLOR 				GLCD_COLOR_LIGHT_GREY // 192, 192, 192
#define TITLE_FORE_COLOR 				GLCD_COLOR_WHITE			// 255, 255, 255
#define STATIC_BACK_COLOR 			GLCD_COLOR_WHITE      // 255, 255, 255
#define STATIC_FORE_COLOR 			0x11E2 								//  16,	 60,  16

extern GLCD_FONT GLCD_Font_16x24;
extern int programIndex;
extern int oldMenuIndex;
extern int menuIndex;
extern AutomaticPump Pump;
extern Time Now;

void Increase(unsigned char *value, unsigned char maxValue);
void Descrease(unsigned char *value, unsigned char maxValue);
void SelectColor(int isSelected);
void ScreenInitialize(char* screenTitle);
