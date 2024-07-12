void Time_update(void);

/*-----------------------------------------------------------------------------
  Periodic timer callback
 *----------------------------------------------------------------------------*/
void Timer_Callback (void const *arg) 
{
    Time_update();
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