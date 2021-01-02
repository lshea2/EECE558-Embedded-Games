#include "systick.h"
#include "redled.h"
#include "greenled.h"
#include "blueled.h"
#include "sw1.h"
#include "copwdt.h"
#include <stdint.h>

void main() {

	asm("CPSID i");
	configure_systick();
	configure_red_led();
	configure_green_led();
	configure_blue_led();
	configure_sw1();
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
		if(counter == 999) { //red
			counter++;
			turn_on_red_led();
		}
		else if(counter == 1999) { //green
			counter++;
			turn_off_red_led();
			turn_on_green_led();
		}
		else if(counter == 2999) { //blue
			counter++;
			turn_off_green_led();
			turn_on_blue_led();
		}
		else if(counter == 3999) { //red/green
			counter++;
			turn_off_blue_led();
			turn_on_red_led();
			turn_on_green_led();
		}
		else if(counter == 4999) { //red/blue
			counter++;
			turn_off_green_led();
			turn_on_blue_led();
		}
		else if(counter == 5999) { //green/blue
			counter++;
			turn_off_red_led();
			turn_on_green_led();
		}
		else if(counter == 6999) { //red/green/blue
			counter++;
			turn_on_red_led();
		}
		else if(counter == 7999) {
			counter = 0;
			turn_off_red_led();
			turn_off_green_led();
			turn_off_blue_led();
		}
		
		feed_the_watchdog();
		
	}

}