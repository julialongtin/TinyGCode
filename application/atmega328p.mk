# Makefile for tinyGCode

# Support package for an atmega328p.

# processor frequency, in Hertz.
CPUFREQ=16000000


####################################################
# You should not have to modify anything below here.
####################################################

MCU_MODEL=atmega328p
MCU_CFLAGS=-mmcu=atmega328p
MCU_DEFINES=-DCPUFREQ=${CPUFREQ}UL
MCU_OBJCOPYFLAGS=-mmcu=atmega328p
