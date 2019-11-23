/* GCODE interpreter */

/* for SPL, SPH. */
#include <avr/io.h>

/* for uint8_t. */
#include <stdint.h>

/* for booleans. */
#include <stdbool.h>

/* for putch. */
#define USE_SERIAL_H 1
#include "serial.h"

/* because we're on a harvard archetecture, not a von-neumann machine. */
#include <avr/pgmspace.h>

/* GCODE for CASE LIGHT:
   M355 S1 P255
   where S is the switch (on, off), and P is the duty cycle (0-255)
   Complying with http://marlinfw.org/docs/gcode/M355.html
*/

void process_gcode(volatile const unsigned char *);

void app_start(uint8_t entry)
{
  /* Set up for running C. */

  /* zero the zero register. */
  asm volatile(
	       "\tclr  __zero_reg__\n"
	       );

  /* set the stack pointer. */
  SPL = (RAMEND-1)&0xFF;
  SPH = (RAMEND-1)>>8;

  /* shut down clocks to unneeded components. */
  PRR = ~(_BV(PRUSART0));
  
  if (entry == 0)
    {
      // setup serial communications
      InitUART();
      /* Enable interrupts. */
      init_putch('J');
      MCUCR = _BV(IVCE);
      MCUCR = 0;
      init_putch('K');
      asm volatile(
		   "\tsei\n"
		   );
  
    }

  /* say we exist. */
  init_putch('A');
  init_putch('0'+entry);
  /* display the reset reason, and reset it. */
  init_putch('R');
  init_putch('0'+MCUSR);
  MCUSR=0;
  /* disable the watchdog timer. */
  WDTCSR=0;
  /* new line. */
  init_putch('\r');
  init_putch('\n');

  /* the inner loop of our program. all work is done in the ISRs, so this should only sleep. */
  for (;;)
    {
      /* Enter sleep mode. */
      SMCR = _BV(SE);
      asm volatile(
		   "\tsleep\n"
		   );
      if (buffer_ready == true)
	{
	  if (process_buffer_0 == true)
	    process_gcode(inbuf0);
	  else
	    process_gcode(inbuf1);
	  putch('>');
	  putch(' ');
	  buffer_ready = false;
	}
      SMCR = 0;
    };
}


/* read a string of one to three characters. */ 
uint16_t matchto3num(volatile const unsigned char * src)
{
  uint8_t i = 0;
  uint8_t count = 0;
  uint16_t res = 0;
  while ((src[count] >= '0') && (src[count] <= '9'))
    count++;
  while ((src[i] >= '0') && (src[i] <= '9'))
    {
      i++;
      res=res+((src[i]-'0')*10^count);
      count--;
    }
  return res;
}

const unsigned char * skipNum(const unsigned char * src)
{
  return src;
}

void process_gcode(volatile const unsigned char * buffer)
{
  uint8_t pos;
  static const unsigned char okmessage[] PROGMEM = "OK\r\n";
  static const unsigned char failmessage1[] PROGMEM = "No GCODE Found\r\n";
  static const unsigned char failmessage2[] PROGMEM = "No G codes supported\r\n";
  volatile const unsigned char * rem;
  uint16_t code;
  /*
  bool switchstate;
  uint8_t duty_cycle; */
  
  pos=0;
  while (buffer[pos] == ' ')
    pos++;
  rem=&buffer[pos];
  if (rem[0] == 'M' || rem[0] == 'G')
    {
      if (rem[0] == 'M')
	{
	  rem++;
	  code = matchto3num(rem);
	  if (code == 355)
	    putch('1');
	  else
	    putch('0' + ((code>>1)&1));
	  puts_P(okmessage);
	}
      else
	puts_P(failmessage2);
    }
  else
    {
      puts_P(failmessage1);
    }
}

