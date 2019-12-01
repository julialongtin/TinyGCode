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

// AMG88xx support for tinyGCode.

#ifndef AMG88xx_H_
#define AMG88xx_H_
#ifdef USE_AMG88XX_H
#else
// in celcius
#define MINTEMP -20
#define MAXTEMP 80
#define RES 0.0625
#endif

void InitAMG(void);

#endif
