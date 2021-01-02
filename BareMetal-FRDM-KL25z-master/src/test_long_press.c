/* Test file for long presses on SW1 on the FRDM-KL25Z development board */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "redled.h"
#include "sw1.h"
#include "copwdt.h"
#include <stdint.h>

#define BOUNCE_INTERVAL 50 //threshold of bounce, in ms
#define LONG_PRESS_INTERVAL 1000 //threshold above which we call long press, in ms

enum SWITCH_PRESS_TYPE { NO_PRESS, SHORT_PRESS, LONG_PRESS};
enum SWITCH_PRESS_TYPE sw1_press_type();

void main() {

	asm("CPSID i");
	configure_systick();
	configure_red_led();
	configure_sw1();
	configure_copwdt();
	asm("CPSIE i");
	
	turn_on_red_led();
	turn_off_red_led();
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		switch(sw1_press_type()) {
			
			case SHORT_PRESS:
				toggle_red_led();
				break;
			case LONG_PRESS:
				turn_off_red_led();
				break;
			case NO_PRESS:
			default:
				break;
				
		}
		
		feed_the_watchdog();
		
	}

}

enum SWITCH_PRESS_TYPE sw1_press_type() {
	
	static enum { ST_NO_PRESS, ST_BOUNCE, ST_SHORT, ST_LONG} state = ST_NO_PRESS;
	static uint16_t ms_cntr;

	switch(state) {
		
		default:
		case ST_NO_PRESS:
			if(sw1_is_pressed()) {
				state = ST_BOUNCE;
				ms_cntr = 0;
			}
			break;
		case ST_BOUNCE:
			if(sw1_is_pressed()) {
				if(ms_cntr < BOUNCE_INTERVAL) {
					ms_cntr++;
				}
				else {
					state = ST_SHORT;
					ms_cntr++;
				}
			}
			else {
				state = ST_NO_PRESS;
			}
			break;
		case ST_SHORT:
			if(!(sw1_is_pressed())) {
				state = ST_NO_PRESS;
				return SHORT_PRESS;
			}
			else {
				if(ms_cntr < LONG_PRESS_INTERVAL) {
					ms_cntr++;
				}
				else {
					state = ST_LONG;
					ms_cntr++;
				}
			}
			break;
		case ST_LONG:
			if(!(sw1_is_pressed())) {
				state = ST_NO_PRESS;
				return LONG_PRESS;
			}
			break;
	}
	
	return NO_PRESS;
	
}