#include "systick.h"
#include "rgbled.h"
#include "copwdt.h"
#include <stdint.h>

void main() {

	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_copwdt();
	asm("CPSIE i");
	
	uint16_t counter = 0;
	uint8_t cntr = 0;
	uint8_t state = 0;
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		if(counter < 2550) {
			cntr = counter / 10;
			if(state == 0) {
				set_rgbled_color_to(cntr,0x00,0x00);
			}
			else if(state == 1) {
				set_rgbled_color_to(0x00,cntr,0x00);
			}
			else if(state == 2) {
				set_rgbled_color_to(0x00,0x00,cntr);
			}
			counter++;
		}
		else {
			counter = 0;
			if(state == 2) {
				state = 0;
			}
			else {
				state++;
			}
		}
		
		feed_the_watchdog();
		
	}

}