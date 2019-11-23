# Makefile for tinyGCode

# the port the device shows up on, for programming.
PROGRAMMER_PORT ?= /dev/ttyUSB1

# the speed the bootloader speaks at. We talk to it to program.
PROGRAMMER_BAUD ?= 57600

PROGRAMMER_TYPE = arduino

####################################################
# You should not have to modify anything below here.
####################################################

PROGRAMMER_ARGS = -b $(PROGRAMMER_BAUD) -P $(PROGRAMMER_PORT)
