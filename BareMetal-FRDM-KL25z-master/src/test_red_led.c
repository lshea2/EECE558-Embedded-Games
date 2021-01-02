/* Test file for red LED and SW1 on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#include "sw1.h"
#include "redled.h"

void main() {
	
	configure_sw1();
	configure_red_led();
	
	while(1) {
		
		if(sw1_is_pressed()) {
			turn_on_red_led();
		}
		else {
			turn_off_red_led();
		}
		
	}
	
}