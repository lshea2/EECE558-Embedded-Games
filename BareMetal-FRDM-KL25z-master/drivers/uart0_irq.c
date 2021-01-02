/* Implementation of driver for the UART0 on the FRDM-KL25Z development board using Interrupts*/
/* Lucas Shea, Binghamton University */
#include "uart0_irq.h"
#include <MKL25Z4.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef UART0_IRQ_PRIORITY
	#define UART0_IRQ_PRIORITY 2
#endif
#define TXBUF_SIZE 32 // can be 2, 4, 8, 16, 32, 64, 128, 256
static uint8_t tx_buf[TXBUF_SIZE];
static uint8_t tx_head, tx_tail;
static uint8_t tx_head_next() {
	return (tx_head + 1) % TXBUF_SIZE;
}
static uint8_t tx_tail_next() {
	return (tx_tail + 1) % TXBUF_SIZE;
}
static _Bool tx_buf_is_empty() {
	return tx_head == tx_tail;
}
static _Bool tx_buf_is_full() {
	return tx_head_next() == tx_tail;
}

static _Bool uart0_rx_ready();

void configure_uart0() {
	
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	SIM->SOPT2 |= (SIM_SOPT2_UART0SRC(1) | SIM_SOPT2_PLLFLLSEL(1));
	PORTA->PCR[1] = (PORTA->PCR[1] & ~(PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(2);
	PORTA->PCR[2] = (PORTA->PCR[2] & ~(PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(2);
	UART0->BDH = 0;
	UART0->BDL = 26; //115.2 kBd
	NVIC_SetPriority(UART0_IRQn,UART0_IRQ_PRIORITY);
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
	UART0->C2 |= (UART0_C2_TE_MASK | UART0_C2_RE_MASK);
	
}

_Bool send_byte_to_uart0(uint8_t data) {
	
	if(!(tx_buf_is_full())) {
		tx_buf[tx_head]=data;
		tx_head = tx_head_next();
		UART0->C2 |= UART0_C2_TIE_MASK;
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

void print_string(char *str) {
	
	while(*str) {
		send_byte_to_uart0(*str++);
	}
	
}

static char to_hex4(uint8_t h) {
	
	h = h & 0x0f;
	if(h < 10) {
		return '0' + h;
	}
	else {
		return 'a' - 10 + h;
	}
	
}

void print_hex8(uint8_t h) {
	
	send_byte_to_uart0(to_hex4(h>>4));
	send_byte_to_uart0(to_hex4(h));
	
}

static _Bool uart0_rx_ready() {
	
	return (UART0->S1 & UART0_S1_RDRF_MASK);
	
}

void UART0_IRQHandler() {
	
	if(!(tx_buf_is_empty())) {
		UART0->D = tx_buf[tx_tail];
		tx_tail = tx_tail_next();
	}
	else {
		UART0->C2 &=~ UART0_C2_TIE_MASK;
	}
	
}