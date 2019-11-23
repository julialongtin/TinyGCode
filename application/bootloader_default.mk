# Makefile for tinyGCode

# applications start in memory at 0000.
APPLICATION_START=0000

# There is a bootloader on this chip.
HAS_BOOTLOADER=True

# The original bootloader is 1024 bytes.
BOOTLOADER_SIZE=1024

# bootloader starts at 3C00
APPLICATION_MAXSIZE=15360


