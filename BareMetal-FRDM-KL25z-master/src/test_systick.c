/* Test file for systick timer and watchdog timer on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "redled.h"
#include "copwdt.h"
#include <stdint.h>

void main() {

	asm("CPSID i");
	configure_systick();
	configure_red_led();
	configure_copwdt();
	asm("CPSIE i");
	
	turn_on_red_led();
	turn_off_red_led();
	
	uint16_t counter = 0;
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		counter++;
		if(counter == 3999) {
			counter = 0;
			toggle_red_led();
		}
		
		feed_the_watchdog();
		
	}

}