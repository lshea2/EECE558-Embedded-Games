#include <stdint.h>
#include <stdbool.h>
#include "rgbled.h"
#include "systick.h"
#include "copwdt.h"
#include "i2c0_irq.h"

void main() {

	uint8_t data[4];
	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_copwdt();
	configure_i2c0();
	asm("CPSIE i");
	
	if(!(i2c0_write_byte(0x1d<<1, 0x2a, 0x07))) {
		set_rgbled_color_to(0xff, 0, 0);
		while(1) {
			feed_the_watchdog();
		}
	}
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		if(i2c0_last_transaction_complete() && (data[0] & (1<<3))) {
			set_rgbled_color_to(data[1], data[2], data[3]);
		}
		i2c0_read_bytes(0x1d<<1, 0x00, 4, data);
		
		feed_the_watchdog();
		
	}

}