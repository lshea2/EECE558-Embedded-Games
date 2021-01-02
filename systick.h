/* Driver header file for systick timer on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#ifndef SYSTICK_H
#define SYSTICK_H

#include <stdbool.h>

void configure_systick();
_Bool systick_has_fired();

#endif