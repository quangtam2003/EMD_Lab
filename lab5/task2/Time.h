#ifndef TIME_H
#define TIME_H

typedef struct
{
	unsigned char Hour;
	unsigned char Minute;
	unsigned char Second;
} Time;

signed char CompareTime(Time t1, Time t2);
void TimeToString(char* string, Time* time);
#endif
