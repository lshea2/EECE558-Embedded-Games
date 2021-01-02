/* Test file to test rgbled driver */
/* Lucas Shea, Binghamton University */
#include "rgbled.h"
#include "copwdt.h"

void main() {

	asm("CPSID i");
	configure_rgbled();
	configure_copwdt();
	asm("CPSIE i");
	
	set_rgbled_color_to(0xFF,0xFF,0xFF);
	
	while(1) {
		
		set_rgbled_color_to(0xFF,0xFF,0xFF);
		feed_the_watchdog();
		
	}

}