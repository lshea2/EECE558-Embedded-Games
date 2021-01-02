#ifndef DAC0_H
#define DAC0_H

#include <stdint.h>

void configure_dac0();
void dac0_write_next_voltage(uint16_t voltage);
void dac0_update_voltage();

#endif