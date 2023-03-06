#include "iridium.h"


int sbd_write(int ser, const void * str){
  int len = strlen(str);

  return write(ser, str, len);
}

int sbd_getline(int ser, char * str, int len){

  int err = -4;

  err = read(ser, str, len);

  if(err < 0){
    int errnum = errno;
    printf("Value of errno: %d\n", errno);
    printf("Error reading: %s\n", strerror( errnum ));
  }

  return err;
}

int sbd_checkid(int ser){
  int length = 15;

  char line[length];
  int n,result;
  tcflush(ser, TCIOFLUSH); //TODO: from http://todbot.com/blog/, may need to sleep(2) before for it to work
  sbd_write(ser, "ati0\r\n");

  usleep(100000);

  //int err = read(ser, line, length);

  int err = sbd_getline(ser, &line,length);	//Should answer "2400"

  if(err < 0){
    printf("Error: sbd_getline returns %d\n", err);
    return -1;
  }

  printf("sbd_checkid: %s\n", line);

  n = sscanf(line,"%i",&result);

  if(n==1 && result==2400){
    return 1;
  } else{
    return -2;
  }
}

int sbd_signal_quality(int ser){
  char line[20];
	int n,result;
  tcflush(ser, TCIOFLUSH); //TODO: from http://todbot.com/blog/, may need to sleep(2) before for it to work
	sbd_write(ser, "at+csq\r\n");
	sbd_getline(ser, line,20);	// "+CSQ:0" to "+CSQ:5"
	n = sscanf(line,"+CSQ:%i",&result);
  printf("Debug: modem response = %s\n", line);
	if (n!=1) return 0;
	return result;
}

int sbd_alloc_outbox(int ser, int size){
	char line[10];
	if( size>340 ) return -1;
	tcflush(ser, TCIOFLUSH); //TODO: from http://todbot.com/blog/, may need to sleep(2) before for it to work
	sprintf(line,"at+sbdwb=%d\r\n",size);
	sbd_write(ser, line);
	sbd_getline(ser, line,10);	// "READY" or "3" invalid size
	if( !strcmp(line,"READY") ) return 1;
	else return -1;
}

int sbd_write_outbox(int ser, void * array, int size){
	char line[10];
	int n,result;
	//uint16_t cs;
	if( size>340 ) return -1;
	unsigned char cs = checksum(array,size);
  sbd_write(ser, array);
  write(ser, cs >> 8, 1); // high byte
  write(ser, cs & 0xFF, 1);  // low  byte

	sbd_getline(ser, line,10);
	// "0" : SBD message successfully written to modem
	// "1" : SBD message write timeout,insufficient bytes were
	//       transferred during the transfer period of 60 seconds
	// "2" : Checksum doesn't match
	n = sscanf(line,"%i",&result);
	return (n==1 && result==0);
}

int sbd_send(int ser){
	char line[80];
	int sent,dummy;
	int n;
	tcflush(ser, TCIOFLUSH); //TODO: from http://todbot.com/blog/, may need to sleep(2) before for it to work
	sbd_write(ser, "at+sbdi\r\n");
	sbd_getline(ser, line,80);
	// +SBDI:<MO status>,<MOMSN>,<MT status>,<MTMSN>,<MT length>,<MT queued>
	// MO status :serial connection
	// "0" No SBD message to send.
	// "1" SBD message successfully sent to the GSS.
	// "2" An error occurred while attempting to send SBD message to GSS.
	n = sscanf(line,"+SBDI:%i,%i,%i,%i,%i,%i",&sent,&dummy,&dummy,&dummy,&dummy,&dummy);
	return (n==6 && sent==1);
}
