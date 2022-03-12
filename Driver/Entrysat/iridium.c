/**
 * @file      iridium.c
 * @date      March 2018
 * @author    Laurent Alloza
 */
//#include <conf_entrysat.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "iridium.h"
//#include "Utilitaires/Utilitaires.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dev/usart.h"
#include "util/clock.h"
#include "portmacro.h"

//	/*init usart 0*/ /*diag*/ /*console*/
//	usart_init(0, cpu_core_clk, B_RATE);
//	/*init usart 1*/ /*connecteur 3 points nanomind A712B-1 (salle grise)*/
//	usart_init(1, cpu_core_clk, B_RATE);

#define CONSOLE_PORT        0 //CONF__C_PORT_CONSOLE
#define IRDM_UART           1 //CONF__C_PORT_IRIDIUM

#define SUCCESS 1
#define FAIL    0

// TimeOuts definitions (ms)
#define IRDM_CHECKID_TIMEOUT	  100
#define IRDM_SIGNALQ_TIMEOUT	10000
#define IRDM_ALLOC_TIMEOUT		  100
#define IRDM_WRITE_TIMEOUT		  500
#define IRDM_SEND_TIMEOUT		19000

/**
 * Compute 16bits checksum of data array
 *
 * @param array : Pointer to data array
 * @param size  : Size of data in bytes
 * @return sum of all bytes in array
 */
static uint16_t checksum(const void * array, int size)
{
int i;
	uint16_t cs = ((uint8_t*)array)[0];
	for(i=1;i<size;i++)	cs += ((uint8_t*)array)[i];
	return cs;
}

/**
 * Write a string to the MODEM and add CR/LF at end of line
 * 
 * @param str : Pointer to null-terminated string
 */
static void sbd_putline(const char * str)
{
	usart_putstr(IRDM_UART, str, strlen(str));
	usart_putc(IRDM_UART, '\r');
	usart_putc(IRDM_UART, '\n');
}
/**
 *  Flush old chars from usart rx queue 
 */
static void sbd_flush(void)
{
	while( usart_messages_waiting(IRDM_UART)>0 ) {
		usart_getc(IRDM_UART);
	}
}
/**
 * Read a line string from the MODEM with timeout.
 * 
 * Stop reading when receiving LF.
 * Remove CR/LF from received string.
 * Return a null-terminated string.
 * On time-out return an empty or partial string.
 *
 * @param str : Pointer to string buffer 
 * @param len : Size of string buffer (must be at least 1 + max string size)
 * @param timeoutms : TimeOut in ms
 */
static void sbd_getline(char *str, int len, unsigned long timeoutms) {
char *c = str;
unsigned long before,elapsed;
unsigned long TicksToWait = timeoutms/portTICK_RATE_MS;
	len--; // reserve 1 char for EOS
	while( len>0 ){
		before = xTaskGetTickCount();
		if( !usart_getc_with_timeout(IRDM_UART, c, TicksToWait ) ){
			*c = 0; // set current char to EOS and exit
			return;
		}
		elapsed = xTaskGetTickCount() - before;
		// process reamining timeout for the entire line
		TicksToWait = ( elapsed < TicksToWait ? TicksToWait-elapsed : 0 );
		// a character has been received
		if( *c=='\r' )	continue;	// skip CR
		if( *c=='\n' ){				// end on LF
			*c = 0; // replace LF to EOS and exit
			return;
		}
		len--;	c++; // next char
	}
	// buffer len reached
	*c = 0;	// replace last char by EOS
}

int sbd_checkid(void){
	char line[10];
	int n,result;
	sbd_flush();
	sbd_putline("ati0");
	sbd_getline(line,10,IRDM_CHECKID_TIMEOUT);	// "2400"
	n = sscanf(line,"%i",&result);
	return (n==1 && result==2400);
}

int sbd_signal_quality(void){
	char line[20];
	int n,result;
	sbd_flush();
	sbd_putline("at+csq");
	sbd_getline(line,20,IRDM_SIGNALQ_TIMEOUT);	// "+CSQ:0" to "+CSQ:5"
	n = sscanf(line,"+CSQ:%i",&result);
	if (n!=1) return 0;
	return result;
}

int sbd_alloc_outbox(int size){
	char line[10];
	if( size>340 ) return FAIL;
	sbd_flush();
	sprintf(line,"at+sbdwb=%d",size);
	sbd_putline(line);
	sbd_getline(line,10,IRDM_ALLOC_TIMEOUT);	// "READY" or "3" invalid size
	if( !strcmp(line,"READY") ) return SUCCESS;
	else return FAIL;
}

int sbd_write_outbox(void * array, int size){
	char line[10];
	int n,result;
	uint16_t cs;
	if( size>340 ) return FAIL;
	cs = checksum(array,size);
	usart_putstr(IRDM_UART, array, size);
	usart_putc(IRDM_UART, cs >> 8);		// high byte
	usart_putc(IRDM_UART, cs & 0xFF);   // low  byte
	sbd_getline(line,10,IRDM_WRITE_TIMEOUT);
	// "0" : SBD message successfully written to modem
	// "1" : SBD message write timeout,insufficient bytes were
	//       transferred during the transfer period of 60 seconds
	// "2" : Checksum doesn't match
	n = sscanf(line,"%i",&result);
	return (n==1 && result==0);
}

int sbd_send(void){
	char line[80];
	int sent,dummy;
	int n;
	sbd_flush();
	sbd_putline("at+sbdi");
	sbd_getline(line,80,IRDM_SEND_TIMEOUT);
	// +SBDI:<MO status>,<MOMSN>,<MT status>,<MTMSN>,<MT length>,<MT queued>
	// MO status :
	// "0" No SBD message to send.
	// "1" SBD message successfully sent to the GSS.
	// "2" An error occurred while attempting to send SBD message to GSS.
	n = sscanf(line,"+SBDI:%i,%i,%i,%i,%i,%i",&sent,&dummy,&dummy,&dummy,&dummy,&dummy);
	return (n==6 && sent==1);
}
