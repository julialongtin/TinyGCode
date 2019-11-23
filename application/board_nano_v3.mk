# Makefile for tinyGCode

# Edit these settings

# Uncomment this line if you want to use the on-board LED.
USE_LED=True

# Uncomment this line if you are using USB to talk to and power the nano.
USE_USB=True

####################################################
# You should not have to modify anything below here.
####################################################

ifneq (${USE_LED},)
# the pin number of the LED on the board. logical pin number, not physical pin number.
LED_PIN=13

# the defines, specifying that we should have gcode support for the on-board LED.
BOARD_DEFINES = -D HAS_CASE_LIGHT -D LED_PIN=$(LED_PIN)

# make sure we compile the LED handling code.
BOARD_TARGETS=led.o
endif

# A flag indicating the board has a crystal on it.
BOARD_HAS_CRYSTAL=True

# the speed of the crystal installed on the PCB.
BOARD_CRYSTAL_SPEED=16000000

ifeq (${USE_USB},True)
BOARD_POWER_USB=True
else
$(error we only support USB power, for now.)
endif

