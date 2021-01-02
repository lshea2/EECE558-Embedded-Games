#include "dac0.h"
#include <stdint.h>
#include <MKL25Z4.h>

#define DAC0_LOC 30

void configure_dac0() {
	
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[DAC0_LOC] = (PORTE->PCR[DAC0_LOC] &~ PORT_PCR_MUX_MASK) | PORT_PCR_MUX(0);
	DAC0->C0 = DAC_C0_DACEN(1) | DAC_C0_DACRFS(0) | DAC_C0_DACTRGSEL(1);
	
}

void dac0_write_next_voltage(uint16_t voltage) {
	
	DAC0->DAT[0].DATL = (voltage & 0xFF);
	DAC0->DAT[0].DATH = (voltage>>8);
	
}

void dac0_update_voltage() {
	
	DAC0->C0 |= DAC_C0_DACSWTRG_MASK;
	
}
	