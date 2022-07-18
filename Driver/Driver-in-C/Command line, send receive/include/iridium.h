#ifndef IRIDIUM_H
#define IRIDIUM_H

//BASED ON CODE WRITTEN BY LAURENT ALLOZA FOR THE ENTRYSAT MISSION

#include "utility.h"

// TimeOuts definitions (tenth of seconds)
#define IRDM_TIMEOUT 50 //default 1

#define BAUD_RATE 115200

/**
 * Write a string to the MODEM
 *
 * @param str : Pointer string that ends in "\r\n"
 */
int sbd_write(int ser, const void * str);


/**
 * Read a line string from the MODEM with timeout.
 *
 * @param str : Pointer to string buffer
 * @param len : Size of string buffer (must be at least 1 + max string size)
 */
int sbd_getline(int ser, char *str, int len);

/**
 * Send command ATI0 (get ident) to the MODEM and check response.
 *
 * @returns 1 on SUCCESS, -1 or -2 on FAIL
 */
int sbd_checkid(int ser);

/**
 * Send command AT+CSQ to the MODEM and check response.
 *
 * @returns The received signal strength indicator (0->5)
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
