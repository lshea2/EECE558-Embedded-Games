#include <stdint.h>
#include "copwdt.h"
#include "systick.h"
#include "dac0.h"

void task_dac_waveform();

#define WAVEFORM_NUMPOINTS 19
const uint16_t waveform[WAVEFORM_NUMPOINTS] = {
	0x000, 0x100, 0x200, 0x300, 0x400, 0x500, 0x600, 0x700, 0x800, 0x900, 0xa00, 0xb00, 0xc00, 0xd00, 0xe00, 0xf00, 0xfff, 0xa00, 0xb00
};

void main() {
	
	asm("CPSID i");
	configure_copwdt();
	configure_systick();
	configure_dac0();
	asm("CPSIE i");
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		task_dac_waveform();
		
		feed_the_watchdog();
		
	}
	
	return;
}

void task_dac_waveform() {
	
	static uint8_t i;
	dac0_write_next_voltage(waveform[i]);
	dac0_update_voltage;
	i = (i==WAVEFORM_NUMPOINTS-1) ? 0 : i+1;
	
}