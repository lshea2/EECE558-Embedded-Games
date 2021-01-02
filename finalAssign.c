/* Final Assignment EECE 558: A long press of the push button switches games. 1st Game: Reaction Time Tester. 2nd Game: Simon Says. Connect to minicom for information display */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "copwdt.h"
#include "rgbled.h"
#include "sw1.h"
#include "tsi0.h"
#include "dma0.h"
#include "tpm1.h"
#include "uart0_irq.h"

#include <stdbool.h>
#include <stdint.h>

#define BOUNCE_INTERVAL 50 //threshold below which we call bounce, in ms
#define SHORT_PRESS_INTERVAL 1000 //threshold below which we call a short press, in ms
#define LED_MAX_BRIGHTNESS 0x05 //real max is 0xff, this value is better for the eyes
#define LED_MIN_BRIGHTNESS 0x00
#define NEWLINE1 0x0a
#define NEWLINE2 0x0d
#define ONESEC 0x03e8
#define TWOSEC 0x07d0
#define THREESEC 0x0bb8
#define FOURSEC 0x0fa0
#define FIVESEC 0x1388
#define HALFSEC 0x01f4
#define ONEHALFSEC 0x05dc
#define TWOHALFSEC 0x09c4
#define THREEHALFSEC 0x0dac
#define FOURHALFSEC 0x1194
#define SIZE_RAND_TIMES_ARRAY 10
#define SIZE_RAND_COLORS_ARRAY 10
#define NUM_RAND_COLORS 3
enum COLORS{RED, GREEN, BLUE};
const enum COLORS dma_colors_array[NUM_RAND_COLORS] = {RED, GREEN, BLUE};
volatile enum COLORS rand_color_dma;

enum SWITCH_PRESS_TYPE { NO_PRESS, SHORT_PRESS, LONG_PRESS};
enum SWITCH_PRESS_TYPE sw1_press_type();

void reaction_time_game();
void simon_says_game();

void main() {

	asm("CPSID i");
	configure_systick();
	configure_copwdt();
	configure_rgbled();
	configure_sw1();
	configure_tsi0();
	configure_dma0(dma_colors_array, &rand_color_dma, NUM_RAND_COLORS);
	configure_tpm1();
	configure_uart0();
	asm("CPSIE i");
	
	start_tpm1();
	
	enum SELECTED_GAME {REACTION_TIME, SIMON_SAYS} game = REACTION_TIME;
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		switch(sw1_press_type()) {
			
			case LONG_PRESS:
				if(game == REACTION_TIME) {
					game = SIMON_SAYS;
					print_string("NOW PLAYING SIMON SAYS");
					send_byte_to_uart0(NEWLINE1);
					send_byte_to_uart0(NEWLINE2);
					turn_off_rgbled();
				}
				else if(game == SIMON_SAYS) {
					game = REACTION_TIME;
					print_string("NOW PLAYING REACTION TIME");
					send_byte_to_uart0(NEWLINE1);
					send_byte_to_uart0(NEWLINE2);
					turn_off_rgbled();
				}
				break;
			default:
			case SHORT_PRESS:
			case NO_PRESS:
				break;
				
		}
		
		switch(game) {
			
			case REACTION_TIME:
				reaction_time_game();
				break;
			case SIMON_SAYS:
				simon_says_game();
				break;
				
		}
		
		feed_the_watchdog();
		
	}

}

enum SWITCH_PRESS_TYPE sw1_press_type() {
	
	static enum {ST_NO_PRESS, ST_BOUNCE, ST_SHORT_PRESS, ST_LONG_PRESS} state = ST_NO_PRESS;
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
					state = ST_SHORT_PRESS;
					ms_cntr++;
				}
			}
			else {
				state = ST_NO_PRESS;
			}
			break;
		case ST_SHORT_PRESS:
			if(!(sw1_is_pressed())) {
				state = ST_NO_PRESS;
				return SHORT_PRESS;
			}
			else {
				if(ms_cntr < SHORT_PRESS_INTERVAL) {
					ms_cntr++;	
				}
				else {		
					state = ST_LONG_PRESS;
					ms_cntr++;
				}
			}
			break;
		case ST_LONG_PRESS:
			if(!(sw1_is_pressed())) {
				state = ST_NO_PRESS;
				return LONG_PRESS;
			}
			break;
			
	}
	
	return NO_PRESS;
	
}

void reaction_time_game() {
	
	const uint16_t times[SIZE_RAND_TIMES_ARRAY] = {
		ONESEC, TWOSEC, THREESEC, FOURSEC, FIVESEC,
		HALFSEC, ONEHALFSEC, TWOHALFSEC, THREEHALFSEC, FOURHALFSEC,
	};
	static enum { ST_LED_OFF, ST_LED_ON, ST_REACTION_BTN_PRESS} reaction_state = ST_LED_OFF;
	static uint16_t reaction_game_ms_cntr = 0;
	static uint16_t reaction_game_highscore = 0xffff;
	static _Bool reaction_game_control_bit = true;
	static uint8_t times_array_index = 0;
	
	switch(reaction_state) {
		
		default:
		case ST_LED_OFF:
			if(reaction_game_ms_cntr < times[times_array_index]) {
				reaction_game_ms_cntr++;
			}
			else {
				set_rgbled_color_to(LED_MAX_BRIGHTNESS, LED_MAX_BRIGHTNESS, LED_MAX_BRIGHTNESS);
				reaction_game_ms_cntr = 0;
				reaction_state = ST_LED_ON;
			}
			break;
		case ST_LED_ON:
			if(!(sw1_is_pressed())) {
				reaction_game_ms_cntr++;
			}
			else {
				reaction_state = ST_REACTION_BTN_PRESS;
			}
			break;
		case ST_REACTION_BTN_PRESS:
			if(reaction_game_control_bit) {
				turn_off_rgbled();
				if(reaction_game_ms_cntr < reaction_game_highscore && reaction_game_ms_cntr != 0) {
					reaction_game_highscore = reaction_game_ms_cntr;
				}
				print_string("Reaction Time (Hex in ms): ");
				print_hex8(reaction_game_ms_cntr>>8);
				print_hex8((uint8_t)reaction_game_ms_cntr);
				reaction_game_control_bit = false;
				reaction_game_ms_cntr = 0;
				times_array_index++;
				if(times_array_index == SIZE_RAND_TIMES_ARRAY) {
					times_array_index = 0;
				}
			}
			if(reaction_game_ms_cntr == 50) { //Spaced out sending to UART to not overflow the buffer
				print_string(" High Score (Hex in ms): ");
				print_hex8(reaction_game_highscore>>8);
				print_hex8((uint8_t)reaction_game_highscore);
				send_byte_to_uart0(NEWLINE1);
				send_byte_to_uart0(NEWLINE2);
				reaction_game_control_bit = true;
				reaction_game_ms_cntr = 0;
				reaction_state = ST_LED_OFF;
			}
			reaction_game_ms_cntr++;
			break;
		
	}
	
}

void simon_says_game() {
	
	static enum {ST_PICK_NEXT_COLOR, ST_DISPLAY_PATTERN, ST_INPUT_PATTERN, ST_WIN_GAME} simon_says_state = ST_PICK_NEXT_COLOR;
	static enum COLORS rand_colors[SIZE_RAND_COLORS_ARRAY] = {RED, RED, RED, RED, RED,
															  RED, RED, RED, RED, RED};
	static enum COLORS scan_result_color = RED;
	static enum COLORS scan_result_highest_ranked_color = RED;												   
	static uint16_t simon_says_game_ms_cntr = 0;
	static uint16_t simon_says_display_total_time_ms_cntr = 0;
	static uint16_t simon_says_display_total_time = 0;
	static uint16_t scan_result = 0;
	static uint8_t colors_array_index = 0;
	static uint8_t current_display_index = 0;
	
	switch(simon_says_state) {
		
		case ST_PICK_NEXT_COLOR:
			if(colors_array_index == SIZE_RAND_COLORS_ARRAY) {
				simon_says_state = ST_WIN_GAME;
				break;
			}
			rand_colors[colors_array_index] = rand_color_dma;
			colors_array_index++;
			simon_says_display_total_time = 1000*colors_array_index;
			simon_says_state = ST_DISPLAY_PATTERN;
			break;
		case ST_DISPLAY_PATTERN:
			if(simon_says_display_total_time_ms_cntr >= simon_says_display_total_time) {
				current_display_index = 0;
				simon_says_display_total_time_ms_cntr = 0;
				simon_says_game_ms_cntr = 0;
				simon_says_state = ST_INPUT_PATTERN;
				break;
			} 
			if(simon_says_game_ms_cntr < 750) {
				if(rand_colors[current_display_index] == RED) {
					set_rgbled_color_to(LED_MAX_BRIGHTNESS, LED_MIN_BRIGHTNESS, LED_MIN_BRIGHTNESS);
				}
				else if(rand_colors[current_display_index] == GREEN) {
					set_rgbled_color_to(LED_MIN_BRIGHTNESS, LED_MAX_BRIGHTNESS, LED_MIN_BRIGHTNESS);
				}
				else if(rand_colors[current_display_index] == BLUE) {
					set_rgbled_color_to(LED_MIN_BRIGHTNESS, LED_MIN_BRIGHTNESS, LED_MAX_BRIGHTNESS);
				}
				simon_says_game_ms_cntr++;
				simon_says_display_total_time_ms_cntr++;
			}
			else if(simon_says_game_ms_cntr < 1000) {
				set_rgbled_color_to(LED_MIN_BRIGHTNESS, LED_MIN_BRIGHTNESS, LED_MIN_BRIGHTNESS);
				simon_says_game_ms_cntr++;
				simon_says_display_total_time_ms_cntr++;
			}
			else {
				current_display_index++;
				simon_says_game_ms_cntr = 0;
			}
			break;
		case ST_INPUT_PATTERN:
			if(current_display_index >= colors_array_index) {
				current_display_index = 0;
				simon_says_state = ST_PICK_NEXT_COLOR;
				break;
			}
			if(simon_says_game_ms_cntr == 0) {
				set_rgbled_color_to(LED_MAX_BRIGHTNESS, LED_MAX_BRIGHTNESS, LED_MAX_BRIGHTNESS);
			}
			if(simon_says_game_ms_cntr == 750) {
				set_rgbled_color_to(LED_MIN_BRIGHTNESS, LED_MIN_BRIGHTNESS, LED_MIN_BRIGHTNESS);
			}
			if(simon_says_game_ms_cntr > 750 && simon_says_game_ms_cntr < 2250) {
				if(tsi0_scan_is_complete()) {
					scan_result = tsi0_get_scan_result();
				}
				if(!(tsi0_scan_is_in_progress())) {
					tsi0_start_scan(10);
				}
				if(scan_result < 40) {
					scan_result = 0;
				}
				else {
					scan_result -=40;
				}
				if(scan_result < 10) {
					scan_result_color = RED;
					turn_off_rgbled();
				}
				else if(scan_result > 10 && scan_result <= 25) {
					scan_result_color = RED;
					set_rgbled_color_to(LED_MAX_BRIGHTNESS, LED_MIN_BRIGHTNESS, LED_MIN_BRIGHTNESS);
				}
				else if(scan_result > 35 && scan_result <= 50) {
					scan_result_color = GREEN;
					set_rgbled_color_to(LED_MIN_BRIGHTNESS, LED_MAX_BRIGHTNESS, LED_MIN_BRIGHTNESS);
				}
				else if(scan_result > 65 && scan_result <=90) {
					scan_result_color = BLUE;
					set_rgbled_color_to(LED_MIN_BRIGHTNESS, LED_MIN_BRIGHTNESS, LED_MAX_BRIGHTNESS);
				}
				if(scan_result_color > scan_result_highest_ranked_color) {
					scan_result_highest_ranked_color = scan_result_color;
				}
			}
			if(simon_says_game_ms_cntr >= 2250) {
				turn_off_rgbled();
				if(scan_result_highest_ranked_color == rand_colors[current_display_index]) {
					current_display_index++;
					scan_result_color = RED;
					scan_result_highest_ranked_color = RED;
					simon_says_game_ms_cntr = 0;
					break;
				}
				else {
					colors_array_index = 0;
					current_display_index = 0;
					scan_result_color = RED;
					scan_result_highest_ranked_color = RED;
					simon_says_game_ms_cntr = 0;
					print_string("YOU LOSE!");
					send_byte_to_uart0(NEWLINE1);
					send_byte_to_uart0(NEWLINE2);
					simon_says_state = ST_PICK_NEXT_COLOR;
					break;
				}
			}
			simon_says_game_ms_cntr++;
			break;
		case ST_WIN_GAME:
			print_string("CONGRATS! YOU WIN!");
			send_byte_to_uart0(NEWLINE1);
			send_byte_to_uart0(NEWLINE2);
			simon_says_game_ms_cntr = 0;
			colors_array_index = 0;
			current_display_index = 0;
			simon_says_state = ST_PICK_NEXT_COLOR;
			break;
		
	}
	
}