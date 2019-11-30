# Makefile for tinyGCode

# Edit these settings, they chose what other makefile fragments to use.

MCU_TARGET    ?= atmega328p
BOOTLOADER    ?= bootloader_optiboot
PROGRAMMER    ?= programmer_usbtinyisp-2.0
#PROGRAMMER    ?= programmer_isp
BOARD_TARGET  ?= board_nano_v3
WIRING_TARGET ?= wiring_default
FUSES         ?= avr_fuses

####################################################
# You should not have to modify anything below here.
####################################################

CC = avr-gcc
LD = avr-ld
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

# add debuging symbols, be verbose and error swiftly, optimize for size, and generate assembly listings using the debuging symbols.
# switch from -Os to -O3 later.
BASE_CFLAGS = -g -Wall -Werror -Wno-error=attributes -Os -std=gnu99 -fno-builtin -Wa,-adhlns=$(<:.c=.lst)

# tell the linker to generate a map file, and use just our code, no outside libraries.
BASE_LDFLAGS = -Map ${PROGRAM}.map --cref -nostartfiles -nostdlib

# We must have a microcontroller.
include ${MCU_TARGET}.mk

# We share that microcontroller with a bootloader.
include ${BOOTLOADER}.mk

# We have a programming method.
include ${PROGRAMMER}.mk

# We may be on a board.
ifneq (${BOARD_TARGET},)
include ${BOARD_TARGET}.mk
endif

# We may have wires connecting the board/microcontroller to sensors.
ifneq (${WIRING_TARGET},)
include ${WIRING_TARGET}.mk
endif

# AVR microcontrollers have fuses.
ifneq (${FUSES},)
include ${FUSES}.mk
endif

CFLAGS=${MCU_CFLAGS} ${BASE_CFLAGS} ${WIRING_DEFINES} ${BOARD_DEFINES}

LDFLAGS=${BASE_LDFLAGS} ${MCU_LDFLAGS}

PROGRAM=tinygcode

COREOBJS=start.o gcode.o serial.o

ALLOBJS=${SENSOR_TARGETS} ${BOARD_TARGETS} ${WIRING_TARGETS} ${COREOBJS}

TARGETS=${PROGRAM}.hex ${PROGRAM}.lss

.PHONY: all flash

all: ${TARGETS}

${PROGRAM}.elf: ${ALLOBJS}
	$(LD) $(LDFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) -j .text -j .data -O ihex $< $@

%.lss: %.elf
	$(OBJDUMP) -h -S $< > $@

flash: ${PROGRAM}.hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) $(PROGRAMMER_ARGS) -p $(MCU_MODEL) -U flash:w:$<

info: ${PROGRAM}.elf
	$(AVRSIZE) -C --mcu=$(MCU_MODEL) $<

install: ${PROGRAM}.hex
	@echo run: $(AVRDUDE) -c $(PROGRAMMER_TYPE) $(PROGRAMMER_ARGS) -p $(MCU_MODEL) -p atmega328p -u -D -U flash:w:$(PROGRAM).hex

clean:
	rm -f $(TARGETS) $(PROGRAM).elf $(PROGRAM).map $(ALLOBJS) *~ *.lst

# our bricked board is set to FF 5C 3F
# our amazon CH340 based nano boards were shipped with fuses: (E:FD, H:DA, L:FF). same as our ebay specials.
shownewfuses:
	@echo run: $(AVRDUDE) -c $(PROGRAMMER_TYPE) $(PROGRAMMER_ARGS) -p $(MCU_MODEL) -u -U efuse:w:0x$(NEW_EFUSE):m -U hfuse:w:0x$(NEW_HFUSE):m -U lfuse:w:0x$(NEW_LFUSE):m
	@echo if you are unsure about these values, check them at: http://www.engbedded.com/fusecalc

showcurrentfuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) $(PROGRAMMER_ARGS) -p $(MCU_MODEL) -nv 
