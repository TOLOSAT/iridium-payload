#ifndef UTILITY_H
#define UTILITY_H

int init_modem(int fd); //From Michael R. Sweet "Serial Programming Guide for POSIX Operating Systems"

unsigned char checksum (unsigned char *ptr, unsigned int sz); //From Stackoverflow user paxdiablo

#endif //UTILITY_H
