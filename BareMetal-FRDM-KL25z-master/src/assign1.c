/* Assignment 1: each time the button is released, the led changes to one of 8 random colors */
/* Lucas Shea, Binghamton University */
#include "redled.h"
#include "greenled.h"
#include "blueled.h"
#include "sw1.h"
#include "copwdt.h"
#include <stdint.h>
#include <stdbool.h>

#define PRESSED true
#define RELEASED false
#define BIT_0_ON 0x01
#define BIT_1_ON 0x02
#define BIT_2_ON 0x04

void main() {

	uint8_t random;
	_Bool last_sw1_state = RELEASED;

	asm("CPSID i");
	configure_red_led();
	configure_green_led();
	configure_blue_led();
	configure_sw1();
	configure_copwdt();
	asm("CPSIE i");
	
	turn_on_red_led();
	turn_on_green_led();
	turn_on_blue_led();
	
	while(1) {
		
		random++;
		if(sw1_is_pressed()) {
			last_sw1_state = PRESSED;
		}
		else {
			if(last_sw1_state == PRESSED) {
				if((random & BIT_0_ON) == BIT_0_ON) {
					turn_on_red_led();
				}
				else {
					turn_off_red_led();	
				}
				
				if((random & BIT_1_ON) == BIT_1_ON) {
					turn_on_green_led();
				}
				else {
					turn_off_green_led();
				}
				
				if((random & BIT_2_ON) == BIT_2_ON) {
					turn_on_blue_led();
				}
				else {
					turn_off_blue_led();
				}
			}
			
			last_sw1_state = RELEASED;
			
		}
		
		feed_the_watchdog();
		
	}

}