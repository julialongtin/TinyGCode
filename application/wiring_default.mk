# Makefile for tinyGCode

# Edit these settings.

# comment out this line to disable AMG88XX support.
USE_AMG88xx=True

# If you're using the AMG88XX, you need pull up resistors somewhere on the TWI bus.
# Leave this uncommented if you want to use the pullup resistors inside of the AVR, instead of supplying your own external resistors.
# Note that many common sensor adaptor boards (melopero, adafruit, ...) have their own pullups already. 
#USE_TWI_PULLUPS=True

# if you're using the AMG88XX, this is the address of the device on the two wire interface:
AMG88XX_ADDR=0x69

# If you're using the AMG88XX, you need to pick a speed for the TWI bus to run at. maximum of 400Khz
TWI_SPEED=100000

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
WIRING_DEFINES += -DHAS_AMG88XX -DUSE_TWI
ifneq (${USE_AVR_TWI_PULLUPS},)
WIRING_DEFINES += -DUSE_TWI_PULLUPS
endif

ifneq (${AMG88XX_ADDR},)
WIRING_DEFINES += -DAMG88XX_ADDR=$(AMG88XX_ADDR)
else
$(error AMG88XX_ADDR must be defined to use an AMG88xx sensor!)
endif

ifneq (${TWI_SPEED},)
WIRING_DEFINES += -DTWI_SPEED=$(TWI_SPEED)
else
$(error TWI_SPEED must be specified to use an AMG88xx sensor!)
endif

endif



