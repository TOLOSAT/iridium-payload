#include "iridium.h"
#include "utility.h"

int main(int argc, char **argv)
{
    //Opens serial port
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    fcntl(fd, F_SETFL, 0); //Blocking read with timeout set by the VTIME parameter

    if(fd < 0){
      printf("Failed to communicate with modem\n");
      return -1;
    }

    serial_options(fd, IRDM_TIMEOUT);

    if( init_modem(fd) < 0 ){
      printf("Failed to communicate with modem\n");
      return -1;
    }

    //Check for command line arguments
    if(argc == 2){

          if(strcmp(argv[1], "checkid") == 0){
            int res = sbd_checkid(fd);
            if(res >= 0){
              printf("Success\n");
            } else{
              printf("Error: sbd_checkid returns %d\n", res);
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
            sbd_write(fd, (void*) argv[2]);

            usleep(100000);

            if(argc == 4){ //./program putline [message] [answer length]

              int len = atoi(argv[3]);

              if(len < 1){

                printf("Error: must specify reply length superior or equal to 0\n");

              } else{

                char ans[len];
                sbd_getline(fd, &ans, len);
                printf("Answer: %s\n", ans);

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

                int res = sbd_send(fd);

                printf("Send returned with code %d\n", res);
              }
            }
          }
    }

    //Closes port
    tcflush(fd, TCIOFLUSH); //TODO: from http://todbot.com/blog/, may need to sleep(2) before for it to work
    close(fd);

    return 0;
}
