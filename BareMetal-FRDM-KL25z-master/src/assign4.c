/* Commands from console control the operations of the RGBLED on the Freedom Development Board */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "rgbled.h"
#include "uart0_irq.h"
#include "copwdt.h"
#include <stdint.h>
#include <stdbool.h>

#define LED_MAX_BRIGHTNESS 255
#define LED_MIN_BRIGHTNESS 0
#define ZERO 0x30
#define NINE 0x39
#define CAPITALA 0x41
#define CAPITALF 0x46
#define LOWERCASEA 0x61
#define LOWERCASEF 0x66
#define NEWLINE1 0x0a
#define NEWLINE2 0x0d

void rgbled_command(uint8_t data);

void main() {

	uint8_t data;
	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_uart0();
	configure_copwdt();
	asm("CPSIE i");
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		if(get_byte_from_uart0(&data)) {
			rgbled_command(data);
		}
		
		feed_the_watchdog();
		
	}

	return;
	
}

void rgbled_command(uint8_t data) {
	
	static enum {ST_SINGLE_CHAR, ST_FIRST_HEX_VAL, ST_SECOND_HEX_VAL} state = ST_SINGLE_CHAR;
	static enum { RED, GREEN, BLUE} selected_color = RED;
	static uint8_t red_led_level = LED_MIN_BRIGHTNESS;
	static uint8_t green_led_level = LED_MIN_BRIGHTNESS;
	static uint8_t blue_led_level = LED_MIN_BRIGHTNESS;
	static uint8_t twoDigitHexValue = 0x00;
	
	switch(state) {
		case ST_SINGLE_CHAR:
			if(data == 'r' || data == 'R') {
				selected_color = RED;
			}
			else if(data == 'g' || data == 'G') {
				selected_color = GREEN;
			}
			else if(data == 'b' || data == 'B') {
				selected_color = BLUE;
			}
			else if(data == 'p' || data == 'R') {
				if(selected_color == RED) {
					print_string("Red: ");
					print_hex8(red_led_level);
					send_byte_to_uart0(NEWLINE1);
					send_byte_to_uart0(NEWLINE2);
				}
				else if(selected_color == GREEN) {
					print_string("Green: ");
					print_hex8(green_led_level);
					send_byte_to_uart0(NEWLINE1);
					send_byte_to_uart0(NEWLINE2);
				}
				else if(selected_color == BLUE) {
					print_string("Blue: ");
					print_hex8(blue_led_level);
					send_byte_to_uart0(NEWLINE1);
					send_byte_to_uart0(NEWLINE2);
				}
			}
			else if(data == 'w' || data == 'W') {
				red_led_level = LED_MAX_BRIGHTNESS;
				green_led_level = LED_MAX_BRIGHTNESS;
				blue_led_level = LED_MAX_BRIGHTNESS;
				set_rgbled_color_to(red_led_level, green_led_level, blue_led_level);
			}
			else if(data == 'o' || data == 'O') {
				red_led_level = LED_MIN_BRIGHTNESS;
				green_led_level = LED_MIN_BRIGHTNESS;
				blue_led_level = LED_MIN_BRIGHTNESS;
				set_rgbled_color_to(red_led_level, green_led_level, blue_led_level);
			}
			else if(data == 'c' || data == 'C') {
				state = ST_FIRST_HEX_VAL;
			}
			break;
		case ST_FIRST_HEX_VAL:
			if(data >= ZERO && data <= NINE) {
				twoDigitHexValue = ((data-'0')<<4);
			}
			else if(data >= CAPITALA && data <= CAPITALF) {
				twoDigitHexValue = ((data-'A'+10)<<4);
			}
			else if(data >= LOWERCASEA && data <= LOWERCASEF) {
				twoDigitHexValue = ((data-'a'+10)<<4);
			}
			else{
				twoDigitHexValue = 0x00;
				state = ST_SINGLE_CHAR;
				return;
			}
			state = ST_SECOND_HEX_VAL;
			break;
		case ST_SECOND_HEX_VAL:
			if(data >= ZERO && data <= NINE) {
				twoDigitHexValue |= (data-'0');
			}
			else if(data >= CAPITALA && data <= CAPITALF) {
				twoDigitHexValue |= (data-'A'+10);
			}
			else if(data >= LOWERCASEA && data <= LOWERCASEF) {
				twoDigitHexValue |= (data-'a'+10);
			}
			else{
				twoDigitHexValue = 0x00;
				state = ST_SINGLE_CHAR;
				return;
			}
			if(selected_color == RED) {
				red_led_level = twoDigitHexValue;
			}
			else if(selected_color == GREEN) {
				green_led_level = twoDigitHexValue;
			}
			else if(selected_color == BLUE) {
				blue_led_level = twoDigitHexValue;
			}
			set_rgbled_color_to(red_led_level, green_led_level, blue_led_level);
			twoDigitHexValue = 0x00;
			state = ST_SINGLE_CHAR;
			break;
	}
	
	return;
	
}