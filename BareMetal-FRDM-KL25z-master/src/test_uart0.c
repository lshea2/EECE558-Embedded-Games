/* Test File for UART0 */
/* Lucas Shea, Binghamton University */
#include <stdint.h>
#include "systick.h"
#include "redled.h"
#include "copwdt.h"
#include "uart0_irq.h"

void main() {

	uint8_t data;
	asm("CPSID i");
	configure_systick();
	configure_red_led();
	configure_uart0();
	configure_copwdt();
	asm("CPSIE i");
	
	turn_on_red_led();
	
	while(1) {
		
		asm("wfi");
		if(!(systick_has_fired())) {
			continue;
		}
		
		if(get_byte_from_uart0(&data)) {
			print_string("Hello: ");
			print_hex8(0xFE);
			send_byte_to_uart0(0x0a);
			send_byte_to_uart0(0x0d);
		}
		
		feed_the_watchdog();
		
	}

}