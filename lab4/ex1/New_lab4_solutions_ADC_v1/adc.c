#include "LPC17xx.h"
#include "adc.h"

// Last converted value
static unsigned int AD_lastValue;
// AD conversion done flag
static unsigned int AD_done;

void ADC_Initialize (void)
{
	// Enable power to ADC & IOCON
  LPC_SC->PCONP |= ((1 << 12) | (1 << 15));
  // Set PINSEL1[19:18] = 01 to select Function AD0.2 for pin P0.25
	LPC_PINCON->PINSEL1 |= 1<<18;
	LPC_PINCON->PINSEL1 &= ~(1<<19);
	// Select AD0.2 pin, ADC clock is 25MHz/5, Enable ADC
  LPC_ADC->ADCR = (1 << 2) | (1 << 10) | (1 << 21);
  // Enable Global ADC interrupt
	LPC_ADC->ADINTEN = (1 <<  8);
	// Enable ADC Interrupt in the NVIC interrupt controller
  NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_IRQHandler(void)
{
	// Read ADC clears interrupt
  int adstat = LPC_ADC->ADSTAT;
	// Store converted value
  AD_lastValue = (LPC_ADC->ADGDR >> 8) & 0xFF;
  AD_done = 1;
}

void ADC_StartConversion (void)
{
	// Stop conversion
  LPC_ADC->ADCR &= ~(7 << 24);
	// Start conversion
  LPC_ADC->ADCR |=  (1 << 24);
}

int ADC_ConversionDone (void)
{
  return AD_done;
}

int ADC_GetValue (void)
{
	return AD_lastValue;
}