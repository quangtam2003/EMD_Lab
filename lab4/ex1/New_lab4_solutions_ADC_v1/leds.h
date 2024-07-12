#include "LPC17xx.h"                        // Device header

#define POWER_CONTROL (LPC_SC->PCONP)

#define PINCONSEL1 (LPC_PINCON->PINSEL1)
#define PINCONSEL2 (LPC_PINCON->PINSEL2)

#define IO1_DIR (LPC_GPIO1->FIODIR)
#define IO2_DIR (LPC_GPIO2->FIODIR)

#define IO1_SET (LPC_GPIO1->FIOSET)
#define IO2_SET (LPC_GPIO2->FIOSET)

#define IO1_CLR (LPC_GPIO1->FIOCLR)
#define IO2_CLR (LPC_GPIO2->FIOCLR)

#define IO1_PIN (LPC_GPIO1->FIOPIN)
#define IO2_PIN (LPC_GPIO2->FIOPIN)

#define LED1_BIT (1 << 6)
#define LED2_BIT (1 << 5)
#define LED3_BIT (1 << 4)
#define LED4_BIT (1 << 3)
#define LED5_BIT (1 << 2)
#define LED6_BIT (1 << 31)
#define LED7_BIT (1 << 29)
#define LED8_BIT (1 << 28)

void leds_init(void);
void led_on(unsigned int index);
void led_off(unsigned int index);
void leds_set_value(char led_Value);
