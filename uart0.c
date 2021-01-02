/* Implementation of driver for the UART0 on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#include "uart0.h"
#include <MKL25Z4.h>
#include <stdbool.h>
#include <stdint.h>

static _Bool uart0_tx_ready();
static _Bool uart0_rx_ready();

void configure_uart0() {
	
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SOPT2 |= (SIM_SOPT2_UART0SRC(1) | SIM_SOPT2_PLLFLLSEL(1));
	PORTA->PCR[1] = (PORTA->PCR[1] & ~(PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(2);
	PORTA->PCR[2] = (PORTA->PCR[2] & ~(PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(2);
	UART0->BDH = 0;
	UART0->BDL = 26; //115.2 kBd
	UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
	
}

_Bool send_byte_to_uart0(uint8_t data) {
	
	if(uart0_tx_ready()) {
		UART0->D = data;
		return true;
	}
	return false;
	
}

_Bool get_byte_from_uart0(uint8_t *data) {
	
	if(uart0_rx_ready()) {
		*data = UART0->D;
		return true;
	}
	return false;
	
}

static _Bool uart0_tx_ready() {
	
	return (UART0->S1 & UART0_S1_TDRE_MASK);
	
}

static _Bool uart0_rx_ready() {
	
	return (UART0->S1 & UART0_S1_RDRF_MASK);
	
}