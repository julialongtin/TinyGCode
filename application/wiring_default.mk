# Makefile for tinyGCode

# Edit these settings.

# comment out this line to disable AMG88XX support.
USE_AMG88xx=True

# Uncomment if you want to use Port C pin 6. This disables RESET.
#USE_PC6=True

# Uncomment if you want to use port B pin 0. This disables the cpu clock from being output on that pin.
USE_PB0=True

# processor frequency, in Hertz.
CPUFREQ=16000000

####################################################
# You should not have to modify anything below here.
####################################################

# we're going to compile support for the AMG88xx

ifneq (${USE_AMG88xx},)
SENSOR_TARGETS  = i2c.o
ifneq (${AMG88xx_ADDR_1},)
SENSOR_TARGETS += amg88xx.o
endif
endif

WIRING_DEFINES=-DCPUFREQ=${CPUFREQ}UL
