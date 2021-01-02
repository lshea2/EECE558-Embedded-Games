/* Implementation of driver for the RGB LED on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#include "rgbled.h"
#include <stdint.h>
#include <MKL25Z4.h>

#define RED_LED_LOC 18 //PORTB[18] TPM2, CH0 MUX3
#define GREEN_LED_LOC 19 //PORTB[19] TPM2, CH1 MUX3
#define BLUE_LED_LOC 1 //PORTD[1] TPM0, CH1 MUX4

void configure_rgbled() {

	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	PTB->PDDR |= (1<<RED_LED_LOC);
	PORTB->PCR[RED_LED_LOC] = (PORTB->PCR[RED_LED_LOC] & ~(PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(3);
	
	PTB->PDDR |= (1<<GREEN_LED_LOC);
	PORTB->PCR[GREEN_LED_LOC] = (PORTB->PCR[GREEN_LED_LOC] & ~(PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(3);
	
	PTD->PDDR |= (1<<BLUE_LED_LOC);
	PORTD->PCR[BLUE_LED_LOC] = (PORTD->PCR[BLUE_LED_LOC] & ~(PORT_PCR_MUX_MASK)) | PORT_PCR_MUX(4);
	
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	TPM2->SC = TPM_SC_CPWMS(0) | TPM_SC_CMOD(0) | TPM_SC_PS(7);
	TPM0->SC = TPM_SC_CPWMS(0) | TPM_SC_CMOD(0) | TPM_SC_PS(7);
	
	TPM2->CONTROLS[0].CnSC = TPM_CnSC_ELSA(1) | TPM_CnSC_MSA(0) | TPM_CnSC_MSB(1);
	TPM2->CONTROLS[1].CnSC = TPM_CnSC_ELSA(1) | TPM_CnSC_MSA(0) | TPM_CnSC_MSB(1);
	TPM0->CONTROLS[1].CnSC = TPM_CnSC_ELSA(1) | TPM_CnSC_MSA(0) | TPM_CnSC_MSB(1);
	
	TPM2->MOD = (255 - 1);
	TPM0->MOD = (255 - 1);

	turn_off_rgbled();
	
	TPM2->SC |= TPM_SC_CMOD(1);
	TPM0->SC |= TPM_SC_CMOD(1);
	
}

void set_rgbled_color_to(uint8_t red, uint8_t green, uint8_t blue) {

	set_red_level(red);
	set_green_level(green);
	set_blue_level(blue);
	
}

void turn_off_rgbled() {

	set_red_level(0);
	set_green_level(0);
	set_blue_level(0);

}

void set_red_level(uint8_t red) {
	
	TPM2->CONTROLS[0].CnV = red;
	
}

void set_green_level(uint8_t green) {
	
	TPM2->CONTROLS[1].CnV = green;
	
}

void set_blue_level(uint8_t blue) {
	
	TPM0->CONTROLS[1].CnV = blue;
	
}
