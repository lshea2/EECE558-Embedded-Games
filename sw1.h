/* Driver header file for SW1 on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#ifndef SW1_H
#define SW1_h

#include <stdbool.h>

void configure_sw1();
_Bool sw1_is_pressed();
_Bool sw1_is_released();
void wait_for_sw1_press();

#endif