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

/* basic initiialization for the arduino. */
#include <avr/io.h>

/* for booleans. */
#include <stdbool.h>

/* We're going to use interrupt driven serial. */
#define USE_SERIAL_H
#include "serial.h"

/* A fake function, containing the interrupt table. */
void start(void) __attribute__ ((section (".vectors")));
void start(void)
{
  // Our interrupt vector table.
  asm volatile(
	       "\trjmp  boot_0\n"           // Hardware reset entry point
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // External Interrupt 0
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // External Interrupt 1
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Pin Change Interrupt Request 0
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Pin Change Interrupt Request 1
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Pin Change Interrupt Request 2
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Watchdog timeout
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 2 Compare Match A
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 2 Compare Match B
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 2 Overflow
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 1 Capture event
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 1 Compare Match A
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 1 Compare Match B
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 1 Overflow
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 0 Compare Match A
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 0 Compare Match B
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Timer/Counter 0 Overflow
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // SPI transfer complete
	       "\tnop\n"
	       "\trjmp  " UART_RX_IRQ "\n"  // USART Rx Complete
	       "\tnop\n"
	       "\trjmp  " UART_TX_IRQ "\n"  // USART Data Register Empty
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // USART Tx Complete
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // ADC Conversion Complete
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // EEPROM Ready
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Analog Comparator
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Two Wire Interface
	       "\tnop\n"
	       "\trjmp  boot_1\n"            // Store Program Memory ready
	       );
}

/* the two boot entry points. */

/* These load a value into R24 indicating which one of them was called, and jump to app_start. */
void boot_0(void) __attribute__((naked));
void boot_0(void)
{
  /* pass control straight to the bootloader */
  asm volatile(
	           "\tldi  R24, 0\n"
		   "\trjmp app_start\n"
	       );
}

void boot_1(void) __attribute__((naked));
void boot_1(void)
{
  asm volatile(
	           "\tldi  R24, 1\n"
		   "\trjmp app_start\n"
	       );
}
