#include "utility.h"

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
    if (write(fd, "AT\r\n", 3) < 3)
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

    if (strncmp(buffer, "OK", 2) == 0)
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
