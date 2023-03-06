#ifndef IRIDIUM_H
#define IRIDIUM_H

//BASED ON CODE WRITTEN BY LAURENT ALLOZA FOR THE ENTRYSAT MISSION

#include "serial.h"
#include <time.h>

// TimeOuts definitions (ms)
#define IRDM_CHECKID_TIMEOUT	  100
#define IRDM_SIGNALQ_TIMEOUT	10000
#define IRDM_ALLOC_TIMEOUT		  100
#define IRDM_WRITE_TIMEOUT		  500
#define IRDM_SEND_TIMEOUT		19000

#define BAUD_RATE 115200


/**
 * Write a string to the MODEM and add CR/LF at end of line
 *
 * @param str : Pointer to null-terminated string
 */
void sbd_putline(int ser, const char * str);


/**
 * Read a line string from the MODEM with timeout.
 *
 * Stop reading when receiving LF.
 * Remove CR/LF from received string.
 * Return a null-terminated string.
 * On time-out return an empty or partial string.
 *
 * @param str : Pointer to string buffer
 * @param len : Size of string buffer (must be at least 1 + max string size)
 * @param timeoutms : TimeOut in ms
 */
void sbd_getline(int ser, char *str, int len, long timeOutMs);

/**
 * Send command ATI0 (get ident) to the MODEM and check response.
 *
 * @returns TRUE (1) on SUCCESS, FALSE (0) on FAIL
 */
int sbd_checkid(int ser);

/**
 * Send command AT+CSQ to the MODEM and check response.
 *
 * @return The received signal strength indicator (0->5)
 */
int sbd_signal_quality(int ser);

/**
 * Allocate the mobile originated buffer to receive data.
 *
 * @param size : Size of message data (max 340 bytes on 9602/9602N)
 * @return TRUE(1) on SUCCESS, FALSE(0) on FAIL
 */
int sbd_alloc_outbox(int ser, int size);

/**
 * Writes binary message to the mobile originated buffer.
 *
 * Buffer can only hold ONE message.
 *
 * @param array : Pointer to message data
 * @param size : Size of message data (max 340 bytes)
 * @return TRUE(1) on SUCCESS, FALSE(0) on FAIL
 */
int sbd_write_outbox(int ser, void * array, int size);

/**
 * Perform SDB session between the MODEM and the GSS.
 *
 * If there is a message in the mobile originated buffer
 * it will be transferred to the GSS.
 * If there is one or more messages queued at the GSS the oldest will be
 * transferred to the MODEM and placed into the mobile terminated buffer.
 *
 * @return TRUE(1) on SUCCESS, FALSE(0) on FAIL
 */
int sbd_send(int ser);

#endif // IRIDIUM_H
