/*  */
/* Lucas Shea, Binghamton University */
#include "systick.h"
#include "rgbled.h"
#include "tpm1.h"
#include "dma0.h"
#include "copwdt.h"
#include <stdint.h>

#define NUM_COLORS 32
const uint16_t colors[NUM_COLORS] = {
	0xf000, 0x0010, 0x1010, 0x0000,
	0xf000, 0x0010, 0x1010, 0x0000,
	0xf000, 0x0010, 0x1010, 0x0000,
	0xf000, 0x0010, 0x1010, 0x0000,
	0xf000, 0x0010, 0x1010, 0x0000,
	0xf000, 0x0010, 0x1010, 0x0000,
	0xf000, 0x0010, 0x1010, 0x0000,
	0xf000, 0x0010, 0x1010, 0xffff
};
volatile uint16_t rg_color;

void main() {

	asm("CPSID i");
	configure_systick();
	configure_rgbled();
	configure_tpm1();
	configure_dma0(colors, &rg_color, NUM_COLORS);
	configure_copwdt();
	asm("CPSIE i");
	
	start_tpm1();
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		set_rgbled_color_to(rg_color>>8, rg_color & 0xff, 0);
		
		feed_the_watchdog();
		
	}

}