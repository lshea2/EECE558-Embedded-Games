#include "tpm1.h"
#include <MKL25Z4.h>

void configure_tpm1() {
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	TPM1->SC = TPM_SC_DMA_MASK | TPM_SC_CPWMS(1) | TPM_SC_CMOD(0) | TPM_SC_PS(7);
	TPM1->MOD = 65535;
	
}

void start_tpm1() {
	
	TPM1->SC |= TPM_SC_CMOD(1);
	
}