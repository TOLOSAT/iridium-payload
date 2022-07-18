#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "iridium.h"
// FreeRTOS header files
//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "dev/arm/cpu_pm.h"
#include "dev/usart.h"
#include "util/driver_debug.h"
//#include "util/byteorder.h"
#include "util/timestamp.h"
#include "util/clock.h"
#include "address.h"



void vTask1(void *pvParameters);
extern void vTaskUsartRx(void *pvParameters);

xTaskHandle xHdlTask1, xHdlTask2;

//#define F_CPU 30000000 // Coreclock
//#define F_OSC 1500000 // Lfclk
#define F_OSC				8000000


int main(void)
{

usart_init(CONSOLE_PORT, cpu_core_clk, B_RATE_CONSOLE); // Port and baud rate defined in address.h
usart_init(IRDM_UART, cpu_core_clk, B_RATE_IRDM); // Port defined in address.h
xTaskCreate(vTask1, (const signed char *) "Task1", 1024*4, NULL, 1, &xHdlTask1);
xTaskCreate(vTaskUsartRx, (const signed char *) "USART", 1024*4, NULL, 3, NULL);

/* The function vTaskStartScheduler shouldn't take any arguments, according to the freeRTOS
* documentation. In this case, GomSpace did some modifications to tune the Simple Timer that
* gives the tick to the RTOS (port.c, function prvSetupTimerInterrupt).
*
* That function has been bypassed by some user defined parameters. Correct working of the
* system is ensured when the CPU is configured according to cpu_pm_pll30(), the option
* configUSE_LF_CLOCK is set, and the tick rate is set to 100 Hz (10 ms). The bypass corrects
* some errors that the GomSpace code introduced, and avoids the use of the math.c library.
*/

vTaskStartScheduler(F_OSC/2, 100);
//
// In case the scheduler returns for some reason, print an error and loop
// forever.
//

while(1)
   {

   }
}

void vTask1(void *pvParameters)
{
	portTickType xLastWakeTime;
	uint8_t message[340];
	bool modemOK = false;
	int i,rssi,success;
	portTickType tic,toc;

	for (i = 1; i <= 340; i++) 	message[i] = i % 256;	// fake message

	for( ; ; ) {
		xLastWakeTime = xTaskGetTickCount();

		do
		{
			tic = xTaskGetTickCount();
			modemOK = sbd_checkid();
			toc = xTaskGetTickCount();
			printf("checkid %i : %ld ms\n",modemOK,toc-tic);
			vTaskDelay( 1000/portTICK_RATE_MS );
		}while( !modemOK );

		do
		{
			tic = xTaskGetTickCount();
			rssi = sbd_signal_quality();
			toc = xTaskGetTickCount();
			printf("rssi %i : %ld ms\n",rssi,toc-tic);
			vTaskDelay( 1000/portTICK_RATE_MS );
		}while( rssi<5 );

		for(i=0;i<3;i++){
			tic = xTaskGetTickCount();
			success = sbd_alloc_outbox(340);
			toc = xTaskGetTickCount();
			printf("alloc %i : %ld ms\n",success,toc-tic);
			if( !success ) break;

			tic = xTaskGetTickCount();
			success = sbd_write_outbox(message,340);
			toc = xTaskGetTickCount();
			printf("write %i : %ld ms\n",success,toc-tic);
			if( !success ) break;

			tic = xTaskGetTickCount();
			success = sbd_send();
			toc = xTaskGetTickCount();
			printf("send %i : %ld ms\n",success,toc-tic);
			if( !success ) break;
		}
		//vTaskDelayUntil( &xLastWakeTime, (20000/portTICK_RATE_MS));
		vTaskDelay( 3000/portTICK_RATE_MS );
	}
}

//void vTask1(void *pvParameters)
//{
//	portTickType xLastWakeTime;
//	xLastWakeTime = xTaskGetTickCount();
//
//	int _cpt;
//	_cpt=0;
//	char varChar;
//	unsigned int statusReg;
//
//	printf("task vTask1\n");
//	for(;;)
//	{
//		varChar = 'B';
//
//		/*attente octet (lecture bloquante)*/
//		varChar = usart_getc(1);
//
//		/*lecture status */
//		statusReg = usart_getStatusReg(1);
//
//		printf("status Reg : %x\n", statusReg);
//		printf("coucou printf %d\n", _cpt);
//
//		/*recopie octet recu*/
//		usart_putc(1,varChar);
//
////		int nb = usart_messages_waiting(1);
////		usart_putstr(1,varChar,1);
////		printf("coucou printf %d\n", _cpt);
////		usart_putstr(0,"coucou1",7);
////		usart_putstr(1,"coucou2",7);
//		_cpt++;
//		vTaskDelayUntil( &xLastWakeTime, (10/portTICK_RATE_MS));
//
//	}
//}


