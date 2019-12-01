# Makefile for tinyGCode

# Edit these settings.

# comment out this line to disable AMG88XX support.
USE_AMG88xx=True

# Uncomment if you want to use Port C pin 6. This disables RESET, and screws with programming through the ISP connector. Know what you are doing!
#USE_PC6=True

# Uncomment if you want to use port B pin 0. This disables the cpu clock from being output on that pin.
USE_PB0=True

# processor frequency, in Hertz.
# We're going to run the chip at full speed.
CPUFREQ=16000000

####################################################
# You should not have to modify anything below here.
####################################################

WIRING_DEFINES= -DCPUFREQ=${CPUFREQ}UL

# we're going to compile support for the AMG88xx

ifneq (${USE_AMG88xx},)
WIRING_TARGETS  = twi.o amg88xx.o
WIRING_DEFINES += -DHAS_AMG88XX
endif



