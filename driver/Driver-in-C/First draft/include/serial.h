//COPIED FROM MICHEL GRIMALDI'S BLOG, UNIVERSITY OF TOULON

//__________________________________________________________________________________
// arduino-serial-lib -- simple library for reading/writing serial ports
//
// 2006-2013, Tod E. Kurt, http://todbot.com/blog/
//
#ifndef SERIAL_H
#define SERIAL_H

#include <stdio.h>    // Standard input/output definitions
#include <unistd.h>   // UNIX standard function definitions
#include <fcntl.h>    // File control definitions
#include <errno.h>    // Error number definitions
#include <termios.h>  // POSIX terminal control definitions
#include <string.h>   // String function definitions
#include <sys/ioctl.h>
#include <stdint.h> // Standard types

// uncomment this to debug reads
//#define SERIALPORTDEBUG

// takes the string name of the serial port (e.g. "/dev/tty.usbserial","COM1")
// and a baud rate (bps) and connects to that port at that speed and 8N1.
// opens the port in fully raw mode so you can send binary data.
// returns valid fd, or -1 on error
int serialport_init(const char* serialport, int baud);

//______________________________________________________________________
int serialport_close( int fd );

//______________________________________________________________________
int serialport_writebyte( int fd, uint8_t b);

//______________________________________________________________________
int serialport_write(int fd, const char* str);

//________________________________________________________________________________
int serialport_read_until(int fd, char* buf, char until, int buf_max, int timeout);

//custom function for tolosat use
int serialport_read_c(int fd, char* c, int timeout)

//___________________________________________________________________________
int serialport_flush(int fd);


#endif //SERIAL_H
