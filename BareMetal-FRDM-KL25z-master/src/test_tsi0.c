/* Test TSI0 */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "rgbled.h"
#include "copwdt.h"
#include "tsi0.h"
#include <stdint.h>

void main() {

	uint16_t scan_result;
	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_tsi0();
	configure_copwdt();
	asm("CPSIE i");
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		if(tsi0_scan_is_complete()) {
			scan_result = tsi0_get_scan_result();
		}
		if(!(tsi0_scan_is_in_progress())) {
			tsi0_start_scan(10);
		}
		if(scan_result < 40) {
			scan_result = 0;
		}
		else {
			scan_result -=40;
		}
		scan_result *= 2;
		set_rgbled_color_to(scan_result & 0xff, 0, 0);
		
		feed_the_watchdog();
		
	}

}