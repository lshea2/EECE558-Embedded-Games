#include "adc0.h"
#include <stdint.h>
#include <stdbool.h>
#include <MKL25Z4.h>

void configure_adc0() {
	
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	
}

void adc0_start_conversion(uint8_t channel) {
	
	ADC0->SC1[0] = ADC_SC1_DIFF(0) | ADC_SC1_AIEN(0) | ADC_SC1_ADCH(channel);
	
}

_Bool adc0_conversion_is_complete() {
	
	return ADC0->SC1[0] & ADC_SC1_COCO_MASK;
	
}

_Bool adc0_conversion_is_in_progress() {
	
	return ADC0->SC2 & ADC_SC2_ADACT_MASK;
	
}

uint8_t adc0_get_result() {
	
	return ADC0->R[0]; //Clears COCO
	
}