# Makefile for tinyGCode

# Edit these settings, they chose what other makefile fragments to use.

MCU_TARGET    ?= atmega328p
BOOTLOADER    ?= bootloader_unknown
PROGRAMMER    ?= programmer_usbtinyisp-2.0
BOARD_TARGET  ?= nanoV3
WIRING_TARGET ?= wiring_default

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
ORIG_CFLAGS = -g -Wall -Werror -Os -Wa,-adhlns=$(<:.c=.lst)

# tell the linker to generate a map file, and use just our code, no outside libraries.
ORIG_LDFLAGS = -Map ${PROGRAM}.map --cref --section-start=.text=$(PROGRAM_START) -nostartfiles -nodefaultlibs

# We must have a microcontroller.
include ${MCU_TARGET}.mk

# We share that microcontroller with a bootloader.
include ${BOOTLOADER}.mk

# We have a programming method.
include ${PROGRAMMER}.mk

# We may be on a board.
ifneq ($(BOARD_TARGET),)
include ${BOARD_TARGET}.mk
endif

# We may have wires connecting the board/microcontroller to sensors.
ifneq ($(WIRING_TARGET),)
include ${WIRING_TARGET}.mk
endif

PROGRAM=tinygcode
TARGETS=init.o serial.o gcode.o

.PHONY: all flash

all: ${PROGRAM}.hex

${PROGRAM}.elf: ${TARGETS} ${SENSOR_TARGETS}
	$(LD) $(LDFLAGS) -o $@ $^

${PROGRAM}.hex: ${PROGRAM}.elf
	$(OBJCOPY) $(OBJCOPYFLAGS) -j .text -j .data -O ihex $< $@

flash: ${PROGRAM}.hex
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) $(PROGRAMMER_ARGS) -p $(MCU_MODEL) -U flash:w:$<

info: ${PROGRAM}.elf
	$(AVRSIZE) -C --mcu=$(MCU_MODEL) $<

showfuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) $(PROGRAMMER_ARGS) -p $(MCU_MODEL) -nv 
