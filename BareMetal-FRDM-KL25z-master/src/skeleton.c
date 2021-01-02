/*  */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "rgbled.h"
#include "sw1.h"
#include "copwdt.h"
#include <stdint.h>

void main() {

	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_sw1();
	configure_copwdt();
	asm("CPSIE i");
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		feed_the_watchdog();
		
	}

}