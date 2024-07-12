#include "LPC17xx.h"                      // Device header
#include "Uart.h"
#include "Led.h"

int main(void)
{
  UART_Initialize(9600);
	Led_Initialize();
	uart_TxChar('h');
	uart_TxChar('e');
  uart_TxChar('l');
  uart_TxChar('l');
  uart_TxChar('o');
	
	
  while(1)
  {
		if(UART_Buffer_Count>0)
		{
			if(UART_Buffer[UART_Buffer_Count-1] == '1')
			{
				Leds_Set_Value(0xFF);
				uart_Send('Turn on the leds',7);
				
			}
			else if(UART_Buffer[UART_Buffer_Count-1] == '0')
			{
				Leds_Set_Value(0x00);
				uart_Send('Turn off the leds',8);
			}
			UART_Buffer_Count = 0;
		}
  }
}
