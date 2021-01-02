/* Test file for i2c0_irq driver: red led flashes and stays off */
/* Lucas Shea, Binghamton University */
#include <stdint.h>
#include "systick.h"
#include "redled.h"
#include "copwdt.h"
#include "i2c0_irq.h"

void main() {

	uint8_t data;
	asm("CPSID i");
	configure_systick();
	configure_red_led();
	configure_copwdt();
	configure_i2c0();
	asm("CPSIE i");
	
	turn_on_red_led();
	while(!(i2c0_read_bytes(0x1d<<1, 0x0d, 1, &data))) {
		;
	}
	while(!(i2c0_last_transaction_complete())) {
		;
	}
	if(data == 0x1a) {
		turn_off_red_led();
	}
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		feed_the_watchdog();
		
	}

}