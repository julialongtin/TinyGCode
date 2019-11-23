# Makefile for tinyGCode

# Support package for an atmega328p.

# Program starts at the beginning of ram, at $0000
PROGRAM_START = 0x0000

####################################################
# You should not have to modify anything below here.
####################################################

SRAM_START = 0x00800100
SRAM_BSS = 0x00800500

MCU_MODEL=atmega328p
MCU_CFLAGS=-mmcu=atmega328p
MCU_LDFLAGS=-section-start=.text=$(PROGRAM_START) -section-start=.data=$(SRAM_START) -section-start=.bss=$(SRAM_BSS) 

# -Tbss $(SRAM_BSS)
MCU_OBJCOPYFLAGS=-mmcu=atmega328p
