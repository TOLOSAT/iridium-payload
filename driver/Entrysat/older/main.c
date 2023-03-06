#include <stdint.h>
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



void vTask1(void *pvParameters);
extern void vTaskUsartRx(void *pvParameters);

xTaskHandle xHdlTask1, xHdlTask2;

#define F_CPU 30000000 // Coreclock
#define F_OSC 1500000 // Lfclk
#define B_RATE 19200


int main(void)
{

cpu_pm_pll30();

usart_init(IRDM_CONSOLE_PORT, F_CPU, B_RATE); // Port defined in iridium.h
usart_init(IRDM_UART, F_CPU, B_RATE); // Port defined in iridium.h
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
        //uint8_t message[3] = {1,2,3};
        uint8_t message[340];
        xLastWakeTime = xTaskGetTickCount();
        int8_t modem;//status of Init function
        int8_t mo;// mo Code of SBDI session
        uint16_t i,isize;
    	timestamp_t MessageTimeStamp;
    	timestamp_t TimebeforeInitcheck;
    	timestamp_t TimeafterInitcheck;
    	timestamp_t TimeafterMessage;

        for (i=0;i<340;i++)
       {
        	message[i]=0;// writing all the values
       }
        isize=sizeof(message);//to check the size of message

  for(;;)
  {
	  clock_get_time(&TimebeforeInitcheck);
	  modem=irdm_Init_Check();
	  clock_get_time(&TimeafterInitcheck);
    mo=irdm_SendMessage(message, sizeof(message),&MessageTimeStamp);
    clock_get_time(&TimeafterMessage);
//    vTaskDelayUntil( &xLastWakeTime, (2000/portTICK_RATE_MS));
  }
}
