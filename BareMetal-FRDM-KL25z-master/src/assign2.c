/* If SW1 is pressed for less than 1s, the selected color is made brighter. If SW1 is pressed for up to 2s, the selected color changes. If SW! is pressed for over 4s, the selected color is turned off. */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "rgbled.h"
#include "sw1.h"
#include "copwdt.h"
#include <stdint.h>

#define BOUNCE_INTERVAL 50 //threshold below which we call bounce, in ms
#define SHORT_PRESS_INTERVAL 1000 //threshold below which we call short press, in ms
#define MEDIUM_PRESS_INTERVAL 2000 //threshold below which we call medium press, in ms
#define LONG_PRESS_INTERVAL 4000 //threshold above which we call long press, in ms

enum SWITCH_PRESS_TYPE { NO_PRESS, SHORT_PRESS, MEDIUM_PRESS, DO_NOTHING, LONG_PRESS};
enum SWITCH_PRESS_TYPE sw1_press_type();

void led_state();
uint8_t red_led_level = 0;
uint8_t green_led_level = 0;
uint8_t blue_led_level = 0;

void main() {

	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_sw1();
	configure_copwdt();
	asm("CPSIE i");
	
	enum SELECTED_COLOR { RED, GREEN, BLUE} color = RED;
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		led_state();
		
		switch(sw1_press_type()) {
			
			case SHORT_PRESS:
				if(color == RED) {
					if((red_led_level + 16) < 255) {
						red_led_level += 16;
					}
					else {
						red_led_level = 255;
					}
				}
				else if(color == GREEN) {
					if((green_led_level + 16) < 255) {
						green_led_level += 16;
					}
					else {
						green_led_level = 255;
					}
				}
				else if (color == BLUE) {
					if((blue_led_level + 16) < 255) {
						blue_led_level += 16;
					}
					else {
						blue_led_level = 255;
					}
				}
				break;
			case MEDIUM_PRESS:
				if(color == RED) {
					color = GREEN;
				}
				else if(color == GREEN) {
					color = BLUE;
				}
				else if (color == BLUE) {
					color = RED;
				}
				break;
			case DO_NOTHING:
				break;
			case LONG_PRESS:
				if(color == RED) {
					red_led_level = 0;
					set_red_level(red_led_level);
				}
				else if(color == GREEN) {
					green_led_level = 0;
					set_green_level(green_led_level);
				}
				else if (color == BLUE) {
					blue_led_level = 0;
					set_blue_level(blue_led_level);
				}
				break;
			case NO_PRESS:
			default:
				break;
		
		}
	
	feed_the_watchdog();
		
	}

	return;

}

enum SWITCH_PRESS_TYPE sw1_press_type() {
	
	static enum { ST_NO_PRESS, ST_BOUNCE, ST_SHORT, ST_MEDIUM, ST_DO_NOTHING, ST_LONG} state = ST_NO_PRESS;
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
					state = ST_MEDIUM;
					ms_cntr++;	
				}
			}
			break;
		case ST_MEDIUM:
			if(!(sw1_is_pressed())) {
				state = ST_NO_PRESS;
				return MEDIUM_PRESS;
			}
			else {
				if(ms_cntr < MEDIUM_PRESS_INTERVAL) {
					ms_cntr++;	
				}
				else {		
					state = ST_DO_NOTHING;
					ms_cntr++;	
				}
			}
			break;
		case ST_DO_NOTHING:
			if(!(sw1_is_pressed())) {
				state = ST_NO_PRESS;
				return DO_NOTHING;
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

void led_state() {
	
	static enum { ST_OFF, ST_ON} state = ST_OFF;
	static uint16_t ms_cntr = 0;
	
	switch(state) {
		
		case ST_OFF:
			if(ms_cntr < 499) {
				turn_off_rgbled();
				ms_cntr++;
			}
			else {
				state = ST_ON;
				set_rgbled_color_to(red_led_level, green_led_level, blue_led_level);
				ms_cntr++;
			}
			break;
		case ST_ON:
			if(ms_cntr < 999) {
				set_rgbled_color_to(red_led_level, green_led_level, blue_led_level);
				ms_cntr++;
			}
			else {
				state = ST_OFF;
				turn_off_rgbled();
				ms_cntr = 0;
			}
			break;
		
	}
	
	return;
	
}