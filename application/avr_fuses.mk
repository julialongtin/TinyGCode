# Makefile for tinyGCode

# This file is responsible for generating the fuse configuration from the configuration options set in the other files.

# Uncomment this to use DebugWire. This will require removing parts and running wires, see: https://www.avrfreaks.net/forum/debugwire-nano-reset-en-solder-bridge
#USE_DEBUGWIRE=True

####################################################
# You should not have to modify anything below here.
####################################################
UNPROGRAMMED=1
PROGRAMMED=0


ifneq (${USE_PC6},)
FUSE_RSTDISBL=${PROGRAMMED}
else
FUSE_RSTDISBL=${UNPROGRAMMED}
endif

ifneq (${USE_PB0},)
FUSE_CKOUT=${UNPROGRAMMED}
else
FUSE_CKOUT=${PROGRAMMED}
endif

ifneq (${USE_DEBUGWIRE},)
FUSE_DWEN=${PROGRAMMED}
else
FUSE_DWEN=${UNPROGRAMMED}
endif

ifeq (${BOARD_POWER_USB},True)
FUSE_BODLEVEL2=${UNPROGRAMMED}
FUSE_BODLEVEL1=${UNPROGRAMMED}
FUSE_BODLEVEL0=${UNPROGRAMMED}
else
$(error we only support USB power, for now.)
endif

# We always want SPI programmability, so the ISP pins work.
FUSE_SPIEN=${PROGRAMMED}

# We do not yet make use of the watchdog timer.
FUSE_WDTON=${UNPROGRAMMED}

# We also do not make use of the EEPROM, yet.
FUSE_EESAVE=${UNPROGRAMMED}

# set the bootloader fuses appropriately.
ifneq (${HAS_BOOTLOADER},)
ifeq (${BOOTLOADER_SIZE},1024)
FUSE_BOOTSZ1=${PROGRAMMED}
FUSE_BOOTSZ0=${UNPROGRAMMED}
else
ifeq (${BOOTLOADER_SIZE},512)
FUSE_BOOTSZ1=${UNPROGRAMMED}
FUSE_BOOTSZ0=${PROGRAMMED}
else
$(error unsupported bootloader size.)
endif
endif
FUSE_BOOTRST=${PROGRAMMED}
endif

ifeq (${BOARD_HAS_CRYSTAL},True)
ifeq (${CPUFREQ},${BOARD_CRYSTAL_SPEED})
# The required positions of the fuses to use the external crystal.
FUSE_CKSEL3=${UNPROGRAMMED}
FUSE_CKSEL2=${UNPROGRAMMED}
FUSE_CKSEL1=${UNPROGRAMMED}
FUSE_CKSEL0=${UNPROGRAMMED}
FUSE_CKDIV8=${UNPROGRAMMED}
FUSE_SUT1=${UNPROGRAMMED}
FUSE_SUT0=${UNPROGRAMMED}
else
$(error Only supporting the external crystal, for now.)
endif
else
$(error Only supporting the external crystal, for now.)
endif

bin2hex=$(shell bash -c "echo \"obase=16;ibase=2;"$(1)"\" | bc")

NEW_EFUSE=$(call bin2hex,11111${FUSE_BODLEVEL2}${FUSE_BODLEVEL1}${FUSE_BODLEVEL0})
NEW_HFUSE=$(call bin2hex,${FUSE_RSTDISBL}${FUSE_DWEN}${FUSE_SPIEN}${FUSE_WDTON}${FUSE_EESAVE}${FUSE_BOOTSZ1}${FUSE_BOOTSZ0}${FUSE_BOOTRST})
NEW_LFUSE=$(call bin2hex,${FUSE_CKDIV8}${FUSE_CKOUT}${FUSE_SUT1}${FUSE_SUT0}${FUSE_CKSEL3}${FUSE_CKSEL2}${FUSE_CKSEL1}${FUSE_CKSEL0})

