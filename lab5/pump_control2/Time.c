#include <stdio.h>
#include "Time.h"

signed char CompareTime(Time t1, Time t2)
{
	if (t1.Hour > t2.Hour)
	{
		return 1;
	}
	else if (t1.Hour < t2.Hour)
	{
		return -1;
	}
	else
	{
		if (t1.Minute > t2.Minute)
		{
			return 1;
		}
		else if (t2.Minute < t2.Minute)
		{
			return -1;
		}
		else
		{
			if (t1.Second > t2.Second)
			{
				return 1;
			}
			else if (t1.Second < t2.Second)
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
	}
}

void TimeToString(char* string, Time* time)
{
	sprintf(string, "%02d:%02d:%02d", time->Hour, time->Minute, time->Second);
}
