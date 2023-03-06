#include "utility.h"

void serial_options(int fd, int timeout_tenth){
  struct termios options;

  //Get the current options for the port

  tcgetattr(fd, &options);

  //Set the baud rates to 115200

  cfsetispeed(&options, B115200);
  cfsetospeed(&options, B115200);

  // set raw input, 1 second timeout

  options.c_cflag     |= (CLOCAL | CREAD);
  options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag     &= ~OPOST;
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = timeout_tenth; // *0.1 seconds

  // set the options
  tcsetattr(fd, TCSANOW, &options);
}

int                  /* O - 0 = MODEM ok, -1 = MODEM bad */
init_modem(int fd)   /* I - Serial port file */
{
  char buffer[255];  /* Input buffer */
  char *bufptr;      /* Current char in buffer */
  int  nbytes;       /* Number of bytes read */
  int  tries;        /* Number of tries so far */

  for (tries = 0; tries < 3; tries ++)
  {
   /* send an AT command followed by a CR */
    if (write(fd, "at\r\n", 3) < 3)
      continue;

   /* read characters into our string buffer until we get a CR or NL */
    bufptr = buffer;
    while ((nbytes = read(fd, bufptr, buffer + sizeof(buffer) - bufptr - 1)) > 0)
    {
      bufptr += nbytes;
      if (bufptr[-1] == '\n' || bufptr[-1] == '\r')
        break;
    }

   /* nul terminate the string and see if we got an OK response */
    *bufptr = '\0';

    printf("buffer = %s\n", buffer);

    if (strncmp(buffer, "\r\nOK", 4) == 0)
      return (0);
  }

  return (-1);
}


unsigned char checksum (unsigned char *ptr, unsigned int sz) {
    unsigned char chk = 0;
    while (sz-- != 0)
        chk -= *ptr++;
    return chk;
}
