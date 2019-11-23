# Makefile for tinyGCode

# applications start in memory at 0000.
APPLICATION_START=0000

# There is a bootloader on this chip.
HAS_BOOTLOADER=True

# The optiboot bootloader is 512 bytes.
BOOTLOADER_SIZE=512

# bootloader starts at 3E00
APPLICATION_MAXSIZE=15872


