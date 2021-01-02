# BareMetal-FRDM-KL25z
Bare Metal Development Environment for FRDM-KL25z Board


This is the bare metal development environment used in the course *Embedded Systems Design* at Binghamton University.  As such, it is intentionally meant to be "empty" in the sense that there are no drivers or example code for the board, other than a simple driver for the red LED (of the RGBLED).  It comprises a generic Makefile that will build a project, a linker script, startup code, and some Freescale header files.  

The Makefile has a configuration define to disable the reset functionality of the single pushbutton (so it can be used as I/O since there are no other switches or buttons).  Because of this, it is possible for someone to "brick" the board by configuring the pin as output and driving the reset value, holding the processor in reset.  Because of this, PE[1] can be grounded on start-up to halt the start-up sequence, allowing the board to be reprogrammed.  If you want to use PE[1] in an application this code needs to be removed from _startup.c.  The Makefile also includes a define to enable/disable the WDT.

The target compiler is arm-none-eabi-gcc.  To build a project, create driver(s) for the desired hardware device(s) in the directory **drivers** and place application code in the **src** subdirectory.  For example, a program test_rgbled.c in **src** that uses the driver files rgbled.c and rgbled.h in **drivers** use the command

LIBS="redled.o" make testredled.srec

Multiple driver .o files are space-separated within the quotes.

Note that the startup code is squeezed into flash memory before the flash configuration block at 0x0400.  If the startup code is modified, it may no longer fit and could over-write the configuration block.  There is no check for this.  Therfore, if the _startup.c is modified this should be verified (it may fail to link but this has not been tested).
