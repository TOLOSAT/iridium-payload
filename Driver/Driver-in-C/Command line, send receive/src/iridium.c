#include "iridium.h"


int sbd_write(int ser, const void * str){
  int len = strlen(str);

  return write(ser, str, len);
}

int sbd_getline_old(int ser, char *str, int len, long int timeOutMs){
  //char * c = str; DON'T THINK USEFUL, CHECK AGAIN IN CASE OF ERROR
  unsigned long int previousTime, elapsed;

  struct timespec spec;

  clock_gettime(CLOCK_REALTIME, &spec);
  previousTime = spec.tv_nsec;

  len--; //reserve one character for EOS

  while(len > 0){

    int err = read(ser, str, 1);

    printf("err = %d, str = %s, time = %ld\n", err, str, previousTime);

    if(err < 0){
      *str = 0; // set current char to EOS and exit

      if(err == -1){
        printf("%s\n", "Couldn't read");
      } else if(err == -2){
        printf("%s\n", "Timeout");
      }

      return err;
    }
    clock_gettime(CLOCK_REALTIME, &spec);
    elapsed = spec.tv_nsec - previousTime;

    previousTime  = elapsed;

    timeOutMs -= elapsed/1e6;

    if(timeOutMs < 0){
      return -3; //timeout
    }

    // a character has been received
    if( *str=='\r' )	continue;	// skip CR
    if( *str=='\n' ){				// end on LF
      *str = 0; // replace LF to EOS and exit
      return 2;
    }
    len--;	str++; // next char
  }
	// buffer len reached
	*str = 0;	// replace last char by EOS

  printf("sbd_getline: %s\n", str);

  return 1;

}

int sbd_getline(int ser, char * str, int len, long timeOutMs){

  long timeOutUs = timeOutMs*1e3;

  struct timeval tval_initial, tval_current, tval_elapsed;


  gettimeofday(&tval_initial, NULL);

  tval_current = tval_initial;

  timersub(&tval_current, &tval_initial, &tval_elapsed);

  int err = -4;

  printf("timeOutUs = %ld \n", timeOutUs);

  while((long int) tval_elapsed.tv_usec < timeOutUs){
      err = read(ser, str, len);

      gettimeofday(&tval_current, NULL);

      timersub(&tval_current, &tval_initial, &tval_elapsed);

      //printf("Elapsed (us): %ld\n", (long int) tval_elapsed.tv_usec);
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

  int err = sbd_getline(ser, line,length,IRDM_CHECKID_TIMEOUT);	//Should answer "2400"

  if(err < 0){
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
	sbd_getline(ser, line,20,IRDM_SIGNALQ_TIMEOUT);	// "+CSQ:0" to "+CSQ:5"
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
	sbd_getline(ser, line,10,IRDM_ALLOC_TIMEOUT);	// "READY" or "3" invalid size
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

	sbd_getline(ser, line,10,IRDM_WRITE_TIMEOUT);
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
	sbd_getline(ser, line,80,IRDM_SEND_TIMEOUT);
	// +SBDI:<MO status>,<MOMSN>,<MT status>,<MTMSN>,<MT length>,<MT queued>
	// MO status :serial connection
	// "0" No SBD message to send.
	// "1" SBD message successfully sent to the GSS.
	// "2" An error occurred while attempting to send SBD message to GSS.
	n = sscanf(line,"+SBDI:%i,%i,%i,%i,%i,%i",&sent,&dummy,&dummy,&dummy,&dummy,&dummy);
	return (n==6 && sent==1);
}
