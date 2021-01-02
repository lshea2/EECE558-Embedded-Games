/* Rainbow color pattern */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "rgbled.h"
#include "copwdt.h"
#include <stdint.h>

void pattern();
uint8_t red_led_level = 0;
uint8_t green_led_level = 0;
uint8_t blue_led_level = 0;
uint16_t pattern_ms_cntr = 0;

void main() {

	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_copwdt();
	asm("CPSIE i");
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		pattern();
		
		feed_the_watchdog();
		
	}

}

void pattern() {
	
	static enum { ST_INCREASE_RED, ST_DECREASE_RED, ST_INCREASE_GREEN, ST_DECREASE_GREEN, ST_INCREASE_BLUE, ST_DECREASE_BLUE, ST_INCREASE_RED_2} state = ST_INCREASE_RED;
	static uint8_t control_bit = 0;
	
	if(control_bit > 0) {
		if(control_bit == 7) {
			control_bit = 0;
			return;
		}
		control_bit++;
		return;
	}
	
	switch(state) {
		
		case ST_INCREASE_RED:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				red_led_level++;
				set_red_level(red_led_level);
			}
			else{
				pattern_ms_cntr = 0;
				state = ST_INCREASE_GREEN;
			}
			break;
		case ST_DECREASE_RED:
			if(pattern_ms_cntr > 0) {
				pattern_ms_cntr--;
				red_led_level--;
				set_red_level(red_led_level);
			}
			else{
				pattern_ms_cntr = 0;
				state = ST_INCREASE_BLUE;
			}
			break;
		case ST_INCREASE_GREEN:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				green_led_level++;
				set_green_level(green_led_level);
			}
			else{
				state = ST_DECREASE_RED;
			}
			break;
		case ST_DECREASE_GREEN:
			if(pattern_ms_cntr > 0) {
				pattern_ms_cntr--;
				green_led_level--;
				set_green_level(green_led_level);
			}
			else{
				pattern_ms_cntr = 0;
				state = ST_INCREASE_RED_2;
			}
			break;
		case ST_INCREASE_BLUE:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				blue_led_level++;
				set_blue_level(blue_led_level);
			}
			else{
				state = ST_DECREASE_GREEN;
			}
			break;
		case ST_DECREASE_BLUE:
			if(pattern_ms_cntr > 0) {
				pattern_ms_cntr--;
				blue_led_level--;
				set_blue_level(blue_led_level);
			}
			else{
				pattern_ms_cntr = 255;
				state = ST_INCREASE_RED;
			}
			break;
		case ST_INCREASE_RED_2:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				red_led_level++;
				set_red_level(red_led_level);
			}
			else{
				state = ST_DECREASE_BLUE;
			}
			break;
		
			
	}
	
	control_bit++;
	
	return;
	
}