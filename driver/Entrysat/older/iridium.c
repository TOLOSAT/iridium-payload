#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#include "iridium.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dev/usart.h"
#include "util/clock.h"

#define IRDM_NEW_LINE 1
#define IRDM_CONTINUE 0
#define IRDM_NO_CONSOLE 2
#define IRDM_TIMEOUT_SBDI 20000// 20 seconds
#define IRDM_TIMEOUT_COUNT 40// 40*500 milliseconds

//Input parameters // M: No need for these global variables
//uint8_t *data;
//uint8_t dataSize;
//uint16_t *moCode, *moMSN, *mtCode, *mtMSN, *mtLen, *mtRemaining;

char* itoa(uint16_t i, char b[])
//-------------------------------
// Converts unsigned integers to a string of
// ASCII characters.
//
// i[in] = integer to convert
// b[out] = integer in ASCII, as a string
//-------------------------------
{
  char const digit[] = "0123456789";
   char* p = b;

   int shifter = i;

   do{ //Move to where representation ends
       ++p;
       shifter = shifter/10;
   }while(shifter);

     *p = '\0';
     do{ //Move back, inserting digits as u go
          *--p = digit[i%10];
          i = i/10;
     }while(i);

     return b;
}

void ConsoleStr(char *msg, uint8_t newLine)
{
  #if IRDM_CONSOLE
   static uint16_t i;
   char number[10];

   if(newLine == IRDM_NEW_LINE) // Adds numbering to commands
   {
      itoa(i, number);
      usart_putc(IRDM_CONSOLE_PORT, '[');
      usart_putstr(IRDM_CONSOLE_PORT, number, strlen(number));
      usart_putc(IRDM_CONSOLE_PORT,']');
      i++;
   }
   usart_putstr(IRDM_CONSOLE_PORT, msg, strlen(msg));
 #endif

}

void ConsoleChar(char data)
{
   #if IRDM_CONSOLE
      usart_putc(IRDM_CONSOLE_PORT, data);
   #endif
}

void SendChar(char data)
// -----------------------
// Auxiliary function to send ASCII chars to Iridium
// -----------------------
{
    //ConsoleChar(data);
    usart_putc(IRDM_UART, data);
}

void SendStr(char *str, uint8_t newLine)
// -----------------------
// Auxiliary function to send ASCII chars to Iridium
// -----------------------
{
      ConsoleStr(str, newLine);
      usart_putstr(IRDM_UART, str, strlen(str));
}

uint8_t irdm_ReadResponse(char *buff)
{
    char resp=' ';
    char respend='\n';
	uint8_t length = 0, i=0, j=0;

	for (j=0; j<IRDM_TIMEOUT_COUNT; j++)
	{
		if (usart_messages_waiting(IRDM_UART)>0)
		{
			do {
//         status = usart_getc_nblock(IRDM_UART, (buff+length), IRDM_TIMEOUT_SBDI);
				resp = usart_getc(IRDM_UART);
				buff[length]=resp;
				length++;
			} while(usart_messages_waiting(IRDM_UART)>0);
		}
		if (resp==respend)
			break;
		vTaskDelay(500 / portTICK_RATE_MS); //Wait 500 ms
	}

  #if IRDM_CONSOLE
     for(i=0; i<length; i++)
       ConsoleChar(*(buff+i));
  #endif

  return length;
}

int8_t irdm_Init_Check()
// --------------------------------------------
// Sends command ATI0 to Iridium modem, reads response,
// and displays it on console (IRDM_CONSOLE_PORT)
//
// Returns: 0 if correct, -1 if not
// Average duration to run: 3 seconds
// --------------------------------------------
{

   vTaskDelay(6000/portTICK_RATE_MS);

   uint8_t i, length;
   int8_t pass=0;
   char response[64], *expectedString = "2400";

   SendStr("ati0\r\n", IRDM_NEW_LINE);
   length = irdm_ReadResponse(response); //Expected response: "2400\r\n"

   if (length>4) {
   for(i=0; i<4; i++)
   {
     if(expectedString[i] != response[i])
     {
        pass = -1; //When response is not correct
        break;
     }
   }
   }
   else
   {
      pass = -1; //When response is not correct
   }
   vTaskDelay(20000/portTICK_RATE_MS);
   return pass;
   }

uint8_t irdm_WriteMessage(uint8_t *data, uint16_t dataSize)
// ----------------------------------------------------
// Writes binary message to the mobile originated buffer.
// Buffer can only hold ONE message.
//
// data [in] = Message to be sent (max size: 340 bytes)
// dataSize [in] = Size of the message in bytes, not
// including the 2-byte checksum
// Returns: Command response from +SDBWB (0-3)
// 0: SBD message successfully written to modem
// 1: SBD message write timeout,insufficient bytes were
//    transferred during the transfer period of 60 seconds
// 2: Checksum doesn't match
// 3: message size not correct(should be between 1-340 bytes)
// ----------------------------------------------------
{
     uint16_t checksum = 0;
     int16_t i;
     char sizeAscii[5], answer[64];
     uint8_t numeric_response;

     itoa(dataSize, sizeAscii); // Converts binary number to ASCII

     SendStr("at+SBDWB=", IRDM_NEW_LINE); // Command to write a Binary Message
     SendStr(sizeAscii, IRDM_CONTINUE);
     SendStr("\r\n", IRDM_CONTINUE);

     irdm_ReadResponse(answer); // Should be "READY"

    for (i=0; i<dataSize; ++i)
    {
        SendChar(data[i]); // Write message byte per byte
        checksum += (uint16_t)data[i];
    }

    SendChar(checksum >> 8); // Send first the MSB of the chksum
    SendChar(checksum & 0xFF);

    irdm_ReadResponse(answer);

    numeric_response = answer[0]-48; //Response can be "0" to "3", convert ASCII to number

   return numeric_response;
}


int8_t irdm_SendSBDI()
// ----------------------------------------------------
// Sends message from Iridium to ground(initiates SBDI session)
//
// moCode [in] = status of message sent(from Iridium modem)
// mtCode[in] = status of message received
// Returns: moCode response to OBC
// ----------------------------------------------------
{
 uint8_t i, j;

 char sbdiResponseBuf[64];
 char *p;
 char answer[64];

 int8_t output;
 uint8_t length = 0;
 uint8_t values[7]; //Elements: +SBDI:, moCode, moMSN, mtCode, mtMSN, mtLen, mtRemaining

   values[1]=3;//initialize mo Code,this will be final mo value if modem doesn't reply

  SendStr("at+CSQ\r\n", IRDM_NEW_LINE);
  irdm_ReadResponse(answer);

   for(i=0; i<IRDM_SEND_TRIES; i++)
   {
       SendStr("at+SBDI\r\n", IRDM_NEW_LINE);
       length = irdm_ReadResponse(sbdiResponseBuf);

       if (length>6){
       // Split response into tokens, delimited by commas
       for(j=0; j<7; j++)
       {
            p = strtok(j == 0 ? sbdiResponseBuf : NULL, ", "); // Breaks the string at ","
            if (p == NULL)
               return IRDM_PROTOCOL_ERROR;
           values[j] = atol(p);
       }

       if(values[1] == 0 || values[1] == 1) // If no message on buffer, or successful transaction, don't retry
         break;
       }
     }
   output = values[1] - 1;
   return output; // Returns moCode value
//   return values[1]; // Returns moCode value
}

int8_t irdm_SendMessage(uint8_t *data, uint16_t dataSize, timestamp_t *timestamp)
// ----------------------------------------------------
// Sends Iridium message to ground
//
// data [in] = Message to be sent (size range 1-340 bytes)
// dataSize [in] = Size of the message
// Returns: Error code, -1 (no SBD message to send), 0 (success), 1 (error in downlink), 2(if modem doesn't reply)
// Return time stamp after the last try to send message:
//  timestamp_t structure: clock since OBC start.

//  ----------------------
/*     uint32_t    tv_sec; in seconds
       uint32_t    tv_nsec; in nanoseconds
    -----------------------

// Average duration to run:8 seconds for 1 SBDI try
// Timeout: 20 seconds
// ----------------------------------------------------*/
{
	int8_t output;

    irdm_WriteMessage(data, dataSize);
    output = irdm_SendSBDI();
	clock_get_time(timestamp); // Timestamp from the OBC right after receiving Iridium data

    return output;
}
