#include "iridium.h"
#include "stdlib.h"

int                  /* O - 0 = MODEM ok, -1 = MODEM bad */
init_modem(int fd)   /* I - Serial port file */
//From Michael R. Sweet "Serial Programming Guide for POSIX Operating Systems"
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


int main(int argc, char **argv)
{
    // ouverture du port à 115200 bauds
    int fd = serialport_init("/dev/ttyUSB0", 115200);
    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);

    struct termios options;

    /*
     * Get the current options for the port...
     */

    tcgetattr(fd, &options);

    /*
     * Set the baud rates to 19200...
     */

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    /* set raw input, 1 second timeout */
    options.c_cflag     |= (CLOCAL | CREAD);
    options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag     &= ~OPOST;
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 10; // 10*0.1 seconds

    /* set the options */
    tcsetattr(fd, TCSANOW, &options);

    fcntl(fd, F_SETFL, FNDELAY);

    if(fd==-1){
      return -1;
    }

    /*
    if( init_modem(fd) < 0 ){
      printf("Failed to communicate with modem\n");
      return -1;
    }
    */



    if(argc == 2){

          if(strcmp(argv[1], "checkid") == 0){
            int res = sbd_checkid(fd);
            if(res >= 0){
              printf("Success\n");
            } else{
              printf("Error\n");
            }

          } else if(strcmp(argv[1], "quality") == 0){
            int res = sbd_signal_quality(fd);
            printf("Signal strength: %d\n", res);

          } else if(strcmp(argv[1], "send") == 0){
            int res = sbd_send(fd);
            if(res){
              printf("Success\n");
            }

          }
      } else if(argc > 2){

          if(strcmp(argv[1], "putline") == 0){ //./program putline [message]
            sbd_putline(fd, (void*) argv[2]);

            if(argc == 6){ //./program putline [message] wait [timeout ms] [answer length]

              if(strcmp(argv[3], "wait") == 0){

                long int to = atoi(argv[4]);

                int len = atoi(argv[5]);

                if(len < 1){

                  printf("Error: must specify reply length superior or equal to 0\n");

                } else{

                  char *ans = (char*) malloc(len*sizeof(char));
                  sbd_getline(fd, ans, len, to);
                  printf("Answer: %s\n", ans);

                }
              }
            }
          } else if(argc == 4 && strcmp(argv[1], "send") == 0){ //./program send [message] size
            void * msg = (void *) argv[2];
            int sz = atoi(argv[3]);

            if(sbd_alloc_outbox(fd, sz) < 0){
              printf("Error: failed to allocate outbox\n");
            } else{

              if(!sbd_write_outbox(fd, msg, sz)){
                printf("Error: failed to write outbox\n");
              } else{
                printf("Error: successfully wrote outbox\n");
              }
            }
          }
    }

    // fermeture du port
    serialport_flush(fd);
    close(fd);

    return 0;
}
