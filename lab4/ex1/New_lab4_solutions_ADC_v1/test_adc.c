#include "LPC17xx.h"
#include "leds.h"
#include "adc.h"

int main()
{
	leds_init();
	ADC_Initialize();
	int ADC_Value;
	char led_value;
	while(1)
	{
		ADC_StartConversion();
		while(ADC_ConversionDone()==0);
		ADC_Value = ADC_GetValue();
		led_value = (char) ADC_Value;
		leds_set_value(led_value);
	}
}