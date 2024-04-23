#include <stdint.h>
#include "FreeRTOS.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "gpio.h"
#include "lcd.h"
//#include "key.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
#define QUEUE_LEN 128

QueueHandle_t xQueue_lcd;
SemaphoreHandle_t xSemaphore_lcd;

static void setupHardware(void)
/*****************************************************************************
*   Input    :  -
*   Output   :  -
*   Function :
*****************************************************************************/
{
  // TODO: Put hardware configuration and initialisation in here

  // Warning: If you do not initialize the hardware clock, the timings will be inaccurate
  init_systick();
  init_gpio();
  xQueue_lcd =XQueueCreate(QUEUE_LEN, sizeof(INT8U));

  xSemaphore_lcd = xSemaphoreCreateMutex();
}

int main(void) {
    // Initialize hardware peripherals (e.g., GPIO) here
    setupHardware();
    // Initialize the LCD module

    xTaskCreate(lcd_task, "LCDTask", configMINIMAL_STACK_SIZE, NULL, LOW_PRIO, NULL); // Add LCD task

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();



    return 0;
}


