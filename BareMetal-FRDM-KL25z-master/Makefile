#Makefile for bare metal ARM development on Freedom FRDM-KL25Z
#Douglas Summerville, Binghamton University, 2018
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
OBJCOPY = arm-none-eabi-objcopy
OBJDUMP = arm-none-eabi-objdump
OBJSIZE = arm-none-eabi-size
INCLUDES = -Idrivers -Ibaremetal
VPATH = src:drivers:baremetal
SYS_CLOCK = 48000000L

LINKSCRIPT=baremetal/mkl25z4.ld 

#OPTS = -O2 "-DSYS_CLOCK=$(SYS_CLOCK)" -DWATCHDOG_DISABLE -DRESET_PIN_DISABLE
OPTS = -O2 "-DSYS_CLOCK=$(SYS_CLOCK)" -DRESET_PIN_DISABLE
TARGET = cortex-m0
CFLAGS = -ffreestanding -nodefaultlibs -nostartfiles \
	 -ffunction-sections -fdata-sections -Wall \
	 -flto -fmessage-length=0 -mcpu=$(TARGET) -mthumb -mfloat-abi=soft \
	 $(DEBUG_OPTS) $(OPTS) $(INCLUDES)
CFLAGSS = -ffreestanding -nodefaultlibs -nostartfiles \
	 -ffunction-sections -fdata-sections -Wall \
	 -fmessage-length=0 -mcpu=$(TARGET) -mthumb -mfloat-abi=soft \
	 $(OPTS) $(INCLUDES)

.PHONY:	clean usage

# -----------------------------------------------------------------------------

usage: 
	@echo LIBS=\"list of drivers\" make file.srec

clean:
	-rm -f *.o *.out *.srec *.dump

_startup.o: _startup.c
	$(CC) $(CFLAGSS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.dump: %.out
	$(OBJDUMP) --disassemble $< >$@

%.srec: %.out
	$(OBJCOPY) -O srec $< $@

#_startup.o must be first in link order- else LTO removes IRQ Handlers
%.out: _startup.o %.o $(LIBS)
	$(CC) $(CFLAGS) -T $(LINKSCRIPT) -o $@ $^
	@echo Generated Program has the following segment sizes:
	@$(OBJSIZE) $@

