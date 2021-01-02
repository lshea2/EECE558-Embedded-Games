/* Test file for red/blue/green LED and SW1 on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#include "sw1.h"
#include "redled.h"
#include "greenled.h"
#include "blueled.h"
#include <stdint.h>

void main() {
	
	asm("CPSID i");
	configure_sw1();
	configure_red_led();
	configure_green_led();
	configure_blue_led();
	asm("CPSIE i");
	
	uint8_t cntr = 0;
	
	while(1) {
		
		if(sw1_is_pressed()) {
			
			if(cntr == 0) {
				cntr++;
			}
			else if(cntr == 1) {
				turn_on_red_led();
				cntr++;
			}
			else if(cntr == 2) {
				turn_off_red_led();
				turn_on_green_led();
				cntr++;
			}
			else if(cntr == 3) {
				turn_off_green_led();
				turn_on_blue_led();
				cntr++;
			}
			else if(cntr == 4) {
				turn_off_blue_led();
				cntr = 0;
			}
			
			while(!(sw1_is_released())) {;}
			
		}
		
		
		
		
	}
	
	return;
}