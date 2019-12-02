/* tinyGCode -- An experimental GCODE interpreter

Copyright (C) 2019 Julia Longtin

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/* AMG88xx support for tinyGCode. */

/* datasheet:
 * https://cdn-learn.adafruit.com/assets/assets/000/043/261/original/Grid-EYE_SPECIFICATIONS(Reference).pdf
 */

/* resources:
(arduino)
https://github.com/sparkfun/SparkFun_GridEYE_Arduino_Library/
https://github.com/linux-downey/Seeed_AMG8833/
https://github.com/jodalyst/AMG8833/
https://github.com/kriswiner/AMG8833/
("MIT license") https://github.com/adafruit/Adafruit_AMG88xx/

(python)
https://github.com/melopero/Melopero_AMG8833
https://github.com/linux-downey/AMG8833_ALGO/

(avr-c)
(GPLv3) https://github.com/michelheil/Arduino/

(stm32)
(nolicense) (2 years) https://github.com/siorpaes/BBIRCamera

(android+esp32)
(nolicense) https://github.com/OberBerger/Thermo-camera-ESP32

(docker?)
(nolicense) https://github.com/ForwardLoopLLC/amg8833/

has a stand-by mode..
so, we need a gcode for setting frames, ...
*/

#ifndef AMG88xx_H_
#define AMG88xx_H_
#ifdef USE_AMG88XX_H
/* extern declarations of shared buffers go here. */
#else
// in celcius
#define MINTEMP -20
#define MAXTEMP 80
#define RES 0.0625
#endif

/* Registers */
#define AMG_PWRCTL     0x00
#define AMG_RST        0x01
#define AMG_FRMRATE    0x02
#define AMG_INTCTL     0x03
#define AMG_STAT       0x04
#define AMG_STATCLR    0x05
#define AMG_THMREGL    0x0E
#define AMG_THMREGH    0x0F
#define AMG_IMG_START  0x80

#define AMG_PWR_NRML   0x00
#define AMG_PWR_SLEEP  0x10
#define AMG_PWR_STBY60 0x20
#define AMG_PWR_STBY10 0x21
#define AMG_RST_FLAG   0x30
#define AMG_RST_INIT   0x3F
#define AMG_FRM_1FPS   0x01
#define AMG_FRM_10FPS  0x00

#define MaybeTWBR(rate, mul) ((CPUFREQ/(rate)-16)/(2*mul))

#if MaybeTWBR(TWI_SPEED, 1) >= 10
#define JustTWBR MaybeTWBR(TWI_SPEED,1)
#define TWPS_MUL 0
#elif MaybeTWBR(TWI_SPEED,4) >= 10
#define JustTWBR MaybeTWBR(TWI_SPEED,4)
#define TWPS_MUL 1
#elif MaybeTWBR(TWI_SPEED,16) >= 10
#define JustTWBR MaybeTWBR(TWI_SPEED,16)
#define TWPS_MUL 2
#elif MaybeTWBR(TWI_SPEED,64) >= 10
#define TWPS_MUL 3
#define JustTWBR MaybeTWBR(TWI_SPEED,64)
#else
#error "TWI Frequency not generatable?"
#endif

void InitAMG(void);

#endif
