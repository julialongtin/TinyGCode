# Makefile for tinyGCode

# Edit these settings.

# comment out this line to disable AMG88XX support.
USE_AMG88xx=True

# specify the i2c address of the first sensor. 68, or 69.
AMG88xx_ADDR_1=68

####################################################
# You should not have to modify anything below here.
####################################################

# we're going to compile support for the AMG88xx

ifneq (${USE_AMG88xx},)
SENSOR_TARGETS  = i2c.o
ifneq (${AMG88xx_ADDR_1},)
SENSOR_DEFINES  = -D AMG88XX_ADDR_1=${AMG88xx_ADDR_1}
SENSOR_TARGETS += amg88xx_1.o
endif

amg88xx_1.o: amg88xx.c
	$(CC) $(CFLAGS) $(SENSOR_DEFINES) $< -o $@

endif
