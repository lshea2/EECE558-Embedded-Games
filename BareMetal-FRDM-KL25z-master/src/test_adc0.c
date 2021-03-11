#include "systick.h"
#include "rgbled.h"
#include "adc0.h"
#include "dac0.h"
#include "copwdt.h"
#include <stdint.h>

void task_output_dac0();

void main() {

	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_adc0();
	configure_dac0();
	configure_copwdt();
	asm("CPSIE i");
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		if(adc0_conversion_is_complete()) {
			set_rgbled_color_to(adc0_get_result(), 0, 0);
		}
		if(!(adc0_conversion_is_in_progress())) {
			adc0_start_conversion(23);
		}
		
		task_output_dac0();
		
		feed_the_watchdog();
		
	}

}

void task_output_dac0() {
	
	static enum {RAMP_UP, RAMP_DOWN} state = RAMP_UP;
	static uint16_t dac_val;
	
	switch(state) {
		default:
		case RAMP_UP:
			dac_val += 2;
			if(dac_val > 0xFF0) {
				state = RAMP_DOWN;
			}
			break;
		case RAMP_DOWN:
			dac_val -= 4;
			if(dac_val < 0x010) {
				state = RAMP_UP;
			}
			break;
			
	}
	
	dac0_write_next_voltage(dac_val);
	dac0_update_voltage();
	
	return;
	
}