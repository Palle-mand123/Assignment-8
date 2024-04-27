#include <stdint.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "lcd.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "tmodel.h"
#include "emp_type.h"
#include "glob_def.h"



volatile int encoder_position = '0';




void init_rotary(void)
{
    volatile int8_t dummy;
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA; // Enable clock for Port A
    dummy = SYSCTL_RCGC2_R; // Read back to introduce a delay

    // Set PA5-PA7 as input pins
    GPIO_PORTA_DIR_R &= ~0xE0;

    // Enable digital functionality for PA5-PA7
    GPIO_PORTA_DEN_R |= 0xE0;

    // Disable analog functionality for PA5-PA7
    GPIO_PORTA_AMSEL_R &= ~0xE0;

    // Unmask the interrupt for PA5
    GPIO_PORTA_IM_R |= 0x20;

    // Configure the interrupt to trigger on both edges for PA5
    GPIO_PORTA_IBE_R |= 0x20;

    
}


void init_interrupt(void)
{
    NVIC_EN0_R |= 0x00010000; // Enable interrupt 16 for GPIO Port A 

    NVIC_PRI4_R = (NVIC_PRI4_R & 0xFF00FFFF) | 0x00600000;


}

void interrupt_handler(void)
{
    static int A = 0; // Store the previous A state
    static int B = 0; // Store the previous B state

    // Check if the interrupt is from PA5
    if(GPIO_PORTA_RIS_R & 0x20) {
        // Clear the interrupt for PA5
        GPIO_PORTA_ICR_R |= 0x20;



        if ((0b00100000) & (GPIO_PORTA_DATA_R)) {
                A = 1;
            } else {
                A = 0;
            }

            // Read the current state of pin A6
            if ((0b01000000) & (GPIO_PORTA_DATA_R)) {
                B = 1;
            } else {
                B = 0;
            }

        if(A==B) {
            // The encoder has moved one position CCW
            encoder_position--;

        } else {
            // The encoder has moved one position CW
            encoder_position++;
        }

        wr_ch_LCD(encoder_position);

        // Toggle the edge detection configuration for PA5
        // Determine if the last interrupt was triggered by a rising or falling edge
        if(GPIO_PORTA_IEV_R & 0x20) {
            // Last interrupt was a rising edge, set to falling edge
            GPIO_PORTA_IEV_R &= ~0x20;
        } else {
            // Last interrupt was a falling edge, set to rising edge
            GPIO_PORTA_IEV_R |= 0x20;
        }


    }

}






void rotary_task(void *pvParameters) {
  init_rotary();
  init_interrupt();

  while(1)
      {
      interrupt_handler();


      }

}

