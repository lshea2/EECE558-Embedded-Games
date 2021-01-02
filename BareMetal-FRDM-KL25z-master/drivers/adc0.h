#ifndef ADC0_H
#define ADC0_H

#include <stdint.h>
#include <stdbool.h>

void configure_adc0();
void adc0_start_conversion(uint8_t channel);
_Bool adc0_conversion_is_complete();
_Bool adc0_conversion_is_in_progress();
uint8_t adc0_get_result();

#endif