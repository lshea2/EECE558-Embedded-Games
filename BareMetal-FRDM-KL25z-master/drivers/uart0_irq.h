#ifndef UART0_IRQ_H
#define UART0_IRQ_H

#include <stdbool.h>
#include <stdint.h>

void configure_uart0();
_Bool send_byte_to_uart0(uint8_t data);
_Bool get_byte_from_uart0(uint8_t *data);
void print_string(char *str);
void print_binary(uint16_t hexInput);
void print_decimal(uint16_t hexInput);
void print_hex8(uint8_t h);

#endif