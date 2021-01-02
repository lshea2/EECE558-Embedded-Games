/* Driver header file for UART0 on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#ifndef UART0_H
#define UART0_H

#include <stdbool.h>
#include <stdint.h>

void configure_uart0();
_Bool send_byte_to_uart0(uint8_t data);
_Bool get_byte_from_uart0(uint8_t *data);

#endif