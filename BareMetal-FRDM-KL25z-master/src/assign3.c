/* If SW1 is pressed for less than 1s, the selected pattern will change. 1st Pattern: All 3 colors increase to max brightness, then decrease to zero. 2nd Pattern: Red increases to max birghtness then red turns off and green
increases to max brightness, then green turns off and blue increases to max brightness. 3rd Pattern: Red increases to max brightness then decreaes to off, green increases to max brightness then decreaes to off, blue increases to max brightness then decreaes to off,*/
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "rgbled.h"
#include "sw1.h"
#include "copwdt.h"
#include <stdint.h>

#define BOUNCE_INTERVAL 50 //threshold below which we call bounce, in ms
#define SHORT_PRESS_INTERVAL 1000 //threshold below which we call short press, in ms

enum SWITCH_PRESS_TYPE { NO_PRESS, SHORT_PRESS, LONG_PRESS};
enum SWITCH_PRESS_TYPE sw1_press_type();

void first_pattern();
void second_pattern();
void third_pattern();

uint8_t red_led_level = 0;
uint8_t green_led_level = 0;
uint8_t blue_led_level = 0;
uint8_t pattern_ms_cntr = 0;
uint8_t timing_control = 0;
uint8_t reset_control = 0;

void main() {

	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_sw1();
	configure_copwdt();
	asm("CPSIE i");
	
	enum SELECTED_PATTERN { FIRST, SECOND, THIRD} pattern = FIRST;
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		switch(sw1_press_type()) {
			
			case SHORT_PRESS:
				if(pattern == FIRST) {
					pattern = SECOND;
					red_led_level = 0;
					green_led_level = 0;
					blue_led_level = 0;
					pattern_ms_cntr = 0;
					timing_control = 0;
					reset_control = 1;
					turn_off_rgbled();
				}
				else if(pattern == SECOND) {
					pattern = THIRD;
					red_led_level = 0;
					green_led_level = 0;
					blue_led_level = 0;
					pattern_ms_cntr = 0;
					timing_control = 0;
					reset_control = 1;
					turn_off_rgbled();
				}
				else if(pattern == THIRD) {
					pattern = FIRST;
					red_led_level = 0;
					green_led_level = 0;
					blue_led_level = 0;
					pattern_ms_cntr = 0;
					timing_control = 0;
					reset_control = 1;
					turn_off_rgbled();
				}
				break;
			case LONG_PRESS:
				break;
			case NO_PRESS:
			default:
				break;
				
		}
		
		switch(pattern) {
			
			case FIRST:
				first_pattern();
				break;
			case SECOND:
				second_pattern();
				break;
			case THIRD:
				third_pattern();
				break;
				
		}
		
		feed_the_watchdog();
		
	}

	return;

}

enum SWITCH_PRESS_TYPE sw1_press_type() {
	
	static enum { ST_NO_PRESS, ST_BOUNCE, ST_SHORT, ST_LONG} state = ST_NO_PRESS;
	static uint16_t ms_cntr = 0;

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
				if(ms_cntr < SHORT_PRESS_INTERVAL) {
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

void first_pattern() {
	
	static enum { ST_INCREASE, ST_DECREASE} state = ST_INCREASE;
	
	if(reset_control == 1) {
		reset_control = 0;
		state = ST_INCREASE;
	}
	
	if(timing_control == 1) {
		timing_control = 0;
		return;
	}
	
	switch(state) {
		
		case ST_INCREASE:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				red_led_level++;
				green_led_level++;
				blue_led_level++;
				set_rgbled_color_to(red_led_level, green_led_level, blue_led_level);
			}
			else{
				state = ST_DECREASE;
			}
			break;
		case ST_DECREASE:
			if(pattern_ms_cntr > 0) {
				pattern_ms_cntr--;
				red_led_level--;
				green_led_level--;
				blue_led_level--;
				set_rgbled_color_to(red_led_level, green_led_level, blue_led_level);
			}
			else{
				state = ST_INCREASE;
			}
			break;
	}
	
	timing_control++;
	
	return;
	
}

void second_pattern() {
	
	static enum { ST_INCREASE_RED, ST_INCREASE_GREEN, ST_INCREASE_BLUE} state = ST_INCREASE_RED;
	
	if(reset_control == 1) {
		reset_control = 0;
		state = ST_INCREASE_RED;
	}
	
	if(timing_control > 0) {
		if(timing_control == 3) {
			timing_control = 0;
			return;
		}
		timing_control++;
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
				state = ST_INCREASE_GREEN;
				pattern_ms_cntr = 0;
				red_led_level = 0;
				set_red_level(red_led_level);
			}
			break;
		case ST_INCREASE_GREEN:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				green_led_level++;
				set_green_level(green_led_level);
			}
			else{
				state = ST_INCREASE_BLUE;
				pattern_ms_cntr = 0;
				green_led_level = 0;
				set_green_level(green_led_level);
			}
			break;
		case ST_INCREASE_BLUE:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				blue_led_level++;
				set_blue_level(blue_led_level);
			}
			else{
				state = ST_INCREASE_RED;
				pattern_ms_cntr = 0;
				blue_led_level = 0;
				set_blue_level(blue_led_level);
			}
			break;
			
	}
	
	timing_control++;
	
	return;
	
}

void third_pattern() {
	
	static enum { ST_INCREASE_RED, ST_DECREASE_RED, ST_INCREASE_GREEN, ST_DECREASE_GREEN, ST_INCREASE_BLUE, ST_DECREASE_BLUE} state = ST_INCREASE_RED;
	
	if(reset_control == 1) {
		reset_control = 0;
		state = ST_INCREASE_RED;
	}
	
	if(timing_control == 1) {
		timing_control = 0;
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
				state = ST_DECREASE_RED;
			}
			break;
		case ST_DECREASE_RED:
			if(pattern_ms_cntr > 0) {
				pattern_ms_cntr--;
				red_led_level--;
				set_red_level(red_led_level);
			}
			else{
				state = ST_INCREASE_GREEN;
			}
			break;
		case ST_INCREASE_GREEN:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				green_led_level++;
				set_green_level(green_led_level);
			}
			else{
				state = ST_DECREASE_GREEN;
			}
			break;
		case ST_DECREASE_GREEN:
			if(pattern_ms_cntr > 0) {
				pattern_ms_cntr--;
				green_led_level--;
				set_green_level(green_led_level);
			}
			else{
				state = ST_INCREASE_BLUE;
			}
			break;
		case ST_INCREASE_BLUE:
			if(pattern_ms_cntr < 255) {
				pattern_ms_cntr++;
				blue_led_level++;
				set_blue_level(blue_led_level);
			}
			else{
				state = ST_DECREASE_BLUE;
			}
			break;
		case ST_DECREASE_BLUE:
			if(pattern_ms_cntr > 0) {
				pattern_ms_cntr--;
				blue_led_level--;
				set_blue_level(blue_led_level);
			}
			else{
				state = ST_INCREASE_RED;
			}
			break;
			
	}
	
	timing_control++;
	
	return;
	
}

