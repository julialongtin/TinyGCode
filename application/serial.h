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

/* serial access functions, header file. */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#define UART_RX_IRQ "__vector_int_rx_uart"
#define UART_TX_IRQ "__vector_int_tx_uart"
#define BAUD_RATE 9600
#define CHARACTER_WIDTH 7
#define STOP_BITS 1

#define CMD_LEN_MAX 13
#define OUTBUFSIZE 100

#ifdef USE_SERIAL_H
/* input buffers */
extern volatile unsigned char inbuf0[CMD_LEN_MAX+1];
extern volatile unsigned char inbuf1[CMD_LEN_MAX+1];
extern volatile bool buffer_ready;
extern volatile bool process_buffer_0;
extern volatile bool do_echo;

#else
/* input buffers */
volatile char unsigned inbuf0[CMD_LEN_MAX+1];
volatile char unsigned inbuf1[CMD_LEN_MAX+1];
volatile bool use_buffer_0;
volatile bool buffer_ready;
volatile bool process_buffer_0;
volatile bool do_echo;

/* our position in the buffer. */
volatile uint8_t inbufpos;

#endif

void InitUART(void);
void init_putch(const unsigned char);
void putch(const unsigned char);
void putch_i(const unsigned char);
uint8_t puts_P(const unsigned char *);
uint8_t my_strlen_P(const unsigned char *);
uint8_t puts_M(const volatile unsigned char *);
uint8_t my_strlen_M(const volatile unsigned char *);

#endif
