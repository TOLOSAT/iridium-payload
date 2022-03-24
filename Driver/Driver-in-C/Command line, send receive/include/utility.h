#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>   /* Standard input/output definitions */
#include "stdlib.h"
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <errno.h>

void serial_options(int fd, int timeout_tenth);

int init_modem(int fd); //From Michael R. Sweet "Serial Programming Guide for POSIX Operating Systems"

unsigned char checksum (unsigned char *ptr, unsigned int sz); //From Stackoverflow user paxdiablo

#endif //UTILITY_H
