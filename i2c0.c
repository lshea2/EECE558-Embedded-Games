#include "i2c0.h"
#include <stdint.h>
#include <stdbool.h>
#include <MKL25Z4.h>

static void i2c0_do_start(uint8_t device_addr);
static void i2c0_do_stop();
static void i2c0_do_repeat_start(uint8_t device_addr);
static void i2c0_put_byte(uint8_t data);
static uint8_t i2c0_get_byte();
static _Bool i2c0_transfer_in_progress();
static void i2c0_wait_for_transfer_to_end();
static void i2c0_nak_after_byte_received();
static _Bool i2c0_ack_was_received();
static void i2c0_ack_after_byte_received();
static _Bool i2c0_is_busy();

_Bool i2c0_write_byte(uint8_t device, uint8_t reg, uint8_t data) {
	
	if(i2c0_is_busy()) {
		return false;
	}
	i2c0_do_start(device | 0x00);
	i2c0_wait_for_transfer_to_end();
	if(!(i2c0_ack_was_received())) {
		goto TX_ERROR;
	}
	i2c0_put_byte(reg);
	i2c0_wait_for_transfer_to_end();
	if(!(i2c0_ack_was_received())) {
		goto TX_ERROR;
	}
	i2c0_put_byte(data);
	i2c0_wait_for_transfer_to_end();
	if(!(i2c0_ack_was_received())) {
		goto TX_ERROR;
	}
	i2c0_do_stop();
	return true;
	
TX_ERROR:
	i2c0_do_stop();
	return false;
	
}

_Bool i2c0_read_bytes(uint8_t device, uint8_t reg, uint8_t count, uint8_t *data) {
	
	if(i2c0_is_busy()) {
		return false;
	}
	i2c0_do_start(device | 0x00);
	i2c0_wait_for_transfer_to_end();
	if(!(i2c0_ack_was_received())) {
		goto RX_ERROR;
	}
	i2c0_put_byte(reg);
	i2c0_wait_for_transfer_to_end();
	if(!(i2c0_ack_was_received())) {
		goto RX_ERROR;
	}
	i2c0_do_repeat_start(device | 0x01);
	i2c0_wait_for_transfer_to_end();
	if(!(i2c0_ack_was_received())) {
		goto RX_ERROR;
	}
	int8_t i = count + 1;
	i2c0_ack_after_byte_received();
	while(i--) {
		if(i == 0) {
			i2c0_do_stop();
		}
		if(i == 1) {
			i2c0_nak_after_byte_received();
		}
		if(i == count) {
			i2c0_get_byte();
		}
		else {
			*data++ = i2c0_get_byte();
		}
		if(i != 0) {
			i2c0_wait_for_transfer_to_end();
		}
	}
	
	return true;
	
RX_ERROR:
	i2c0_do_stop();
	return false;
	
}

void configure_i2c0() {
	
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	PORTE->PCR[24] = (PORTE->PCR[24] &! PORT_PCR_MUX_MASK) | PORT_PCR_MUX(5);
	PORTE->PCR[25] = (PORTE->PCR[25] &! PORT_PCR_MUX_MASK) | PORT_PCR_MUX(5);
	I2C0->F = I2C_F_ICR(0x12) | I2C_F_MULT(0);
	I2C0->C1 |= I2C_C1_IICEN_MASK;
	
}

static void i2c0_do_start(uint8_t device_addr) {
	
	I2C0->C1 |= (I2C_C1_TX_MASK | I2C_C1_MST_MASK);
	I2C0->D = device_addr; 
	
}

static void i2c0_do_stop() {
	
	I2C0->C1 &= ~I2C_C1_MST_MASK;
	
}

static void i2c0_do_repeat_start(uint8_t device_addr) {
	
	I2C0->C1 |= (I2C_C1_TX_MASK | I2C_C1_RSTA_MASK);
	I2C0->D = device_addr; 
	
}

static void i2c0_put_byte(uint8_t data) {
	
	I2C0->D = data;
	
}

static uint8_t i2c0_get_byte() {
	
	I2C0->C1 &= ~I2C_C1_TX_MASK;
	return I2C0->D;
	
}

static _Bool i2c0_transfer_in_progress() {
	
	return (I2C0->S & I2C_S_IICIF_MASK);
	
}

static void i2c0_wait_for_transfer_to_end() {
	
	while(!i2c0_transfer_in_progress() ) 
		;
	I2C0->S |= I2C_S_IICIF_MASK;
	
}

static void i2c0_nak_after_byte_received() {
	
	I2C0->C1 |= I2C_C1_TXAK_MASK;
	
}

static _Bool i2c0_ack_was_received() {
	
	return !(I2C0->S & I2C_S_RXAK_MASK);
	
}

static void i2c0_ack_after_byte_received() {
	
	I2C0->C1 &= ~I2C_C1_TXAK_MASK;
	
}

static _Bool i2c0_is_busy() {
	
	return (I2C0->S & I2C_S_BUSY_MASK);
	
}