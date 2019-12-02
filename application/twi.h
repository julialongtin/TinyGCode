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

/* two wire interfacing. */

#ifndef TWI_H_
#define TWI_H_
#ifdef USE_TWI_H

#else

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

#endif

void InitTWI();

#endif

