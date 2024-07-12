/*-----------------------------------------------------------------------------
  Increase and Decrease
 *----------------------------------------------------------------------------*/
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

void Decrease(unsigned char *value, unsigned char maxValue)
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