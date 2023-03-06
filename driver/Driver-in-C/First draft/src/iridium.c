#include "iridium.h"


static void sbd_putline(int ser, const char * str){
  serialport_write(ser, str);
  serialport_write(ser, "\r\n");
}

static void sbd_getline(int ser, char *str, int len, long timeOutMs){
  //char * c = str; DON'T THINK USEFUL, CHECK AGAIN IN CASE OF ERROR
  long previousTime, elapsed;

  struct timespec spec;



  len--; //reserve one character for EOS

  while(len > 0){
    clock_gettime(CLOCK_REALTIME, &spec);
    previousTime = spec.tv_nsec;
    if( serialport_read_c(ser, str, timeOutMs) < 0){
      *str = 0; // set current char to EOS and exit
      return;
    }
    clock_gettime(CLOCK_REALTIME, &spec);
    elapsed = spec.tv_nsec - previousTime;

    timeOutMs -= elapsed/1e6;

    if(timeOutMs < 0){
      return -1; //timeout
    }

    // a character has been received
    if( *str=='\r' )	continue;	// skip CR
    if( *str=='\n' ){				// end on LF
      *str = 0; // replace LF to EOS and exit
      return;
    }
    len--;	str++; // next char
  }
	// buffer len reached
	*c = 0;	// replace last char by EOS

  }
}

static int sbd_checkid(int ser){
  char line[10];
  int n,result;
  serialport_flush(ser);
  sbd_putline(ser, "ati0");
  sbd_getline(ser, line,10,IRDM_CHECKID_TIMEOUT);	//Should answer "2400"

  n = sscanf(line,"%i",&result);

  return (n==1 && result==2400);
}

int sbd_signal_quality(int ser){
  char line[20];
	int n,result;
	serialport_flush(ser);
	sbd_putline(ser, "at+csq");
	sbd_getline(ser, line,20,IRDM_SIGNALQ_TIMEOUT);	// "+CSQ:0" to "+CSQ:5"
	n = sscanf(line,"+CSQ:%i",&result);
	if (n!=1) return 0;
	return result;
}

int sbd_alloc_outbox(int size){
	char line[10];
	if( size>340 ) return FAIL;
	serialport_flush(ser);
	sprintf(line,"at+sbdwb=%d",size);
	sbd_putline(ser, line);
	sbd_getline(ser, line,10,IRDM_ALLOC_TIMEOUT);	// "READY" or "3" invalid size
	if( !strcmp(line,"READY") ) return SUCCESS;
	else return FAIL;
}

int sbd_write_outbox(void * array, int size){
	char line[10];
	int n,result;
	uint16_t cs;
	if( size>340 ) return FAIL;
	cs = checksum(array,size);
  sbd_putline(ser, array);
  serialport_writebyte(ser, cs >> 8); // high byte
  serialport_writebyte(ser, cs & 0xFF);  // low  byte

	sbd_getline(ser, line,10,IRDM_WRITE_TIMEOUT);
	// "0" : SBD message successfully written to modem
	// "1" : SBD message write timeout,insufficient bytes were
	//       transferred during the transfer period of 60 seconds
	// "2" : Checksum doesn't match
	n = sscanf(line,"%i",&result);
	return (n==1 && result==0);
}

int sbd_send(void){
	char line[80];
	int sent,dummy;
	int n;
	serialport_flush(ser);
	sbd_putline(ser, "at+sbdi");
	sbd_getline(ser, line,80,IRDM_SEND_TIMEOUT);
	// +SBDI:<MO status>,<MOMSN>,<MT status>,<MTMSN>,<MT length>,<MT queued>
	// MO status :
	// "0" No SBD message to send.
	// "1" SBD message successfully sent to the GSS.
	// "2" An error occurred while attempting to send SBD message to GSS.
	n = sscanf(line,"+SBDI:%i,%i,%i,%i,%i,%i",&sent,&dummy,&dummy,&dummy,&dummy,&dummy);
	return (n==6 && sent==1);
}
