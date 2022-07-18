#include "serial.h"
#include "iridium.c"

int main(int argc, char **argv)
{
    char buffer[100];                   // un buffer
    int i;

    // ouverture du port à 115200 bauds
    int fd = serialport_init("/dev/ttyUSB0", 115200);
    if (fd==-1) return -1;

    // boucle
    for ( ; ; ){
        //  lecture d'une ligne
        serialport_read_until(fd, buffer, '\r', 99, 10000);

        // suppression de la fin de ligne
        for (i=0 ; buffer[i]!='\r' && i<100 ; i++);
        buffer[i] = 0;

        // écriture du résultat
        printf("%s", buffer);
    }

    // fermeture du port
    serialport_flush(fd);
    serialport_close(fd);

    return 0;
}
