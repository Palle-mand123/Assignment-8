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


volatile INT16U encoder_position = 0;

extern QueueHandle_t xQueue_lcd;



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

     // Make interrupt edge-triggered for PA5
    GPIO_PORTA_IS_R &= ~0x20;

    // Configure the interrupt to trigger on both edges for PA5
    GPIO_PORTA_IBE_R |= 0x20;

      // Unmask the interrupt for PA5
    GPIO_PORTA_IM_R |= 0x20;


}


void init_interrupt(void)
{
    NVIC_EN0_R |= 0x00010000; // Enable interrupt 16 for GPIO Port A

    NVIC_PRI4_R = (NVIC_PRI4_R & 0xFF00FFFF) | 0x00600000;


}

void send_encoder_position(INT16U p)
{
    if(p !=0)
    {
    wr_ch_LCD(p / 10000 + '0');
    p = p % 10000;
    wr_ch_LCD(p / 1000 + '0');
    p = p % 1000;
    wr_ch_LCD(p / 100 + '0');
    p = p % 100;
    wr_ch_LCD(p / 10 + '0');
    p = p % 10;
    wr_ch_LCD(p + '0');
    }

}

void interrupt_handler(void)
{
    static int prevA=0;
    static int prevB=0;
    static int A ;
    static int B ;

    // Check if the interrupt is from PA5
    if(GPIO_PORTA_RIS_R & 0x20) {






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

            int AB = (A << 1) | B;

            int prevAB = (prevA << 1)| prevB;

            INT8U YY = AB ^ prevAB;



        if(A==B) {
            if(YY==0x01)
            {
                encoder_position++;
            }else if(YY==0x02)
            {
                encoder_position--;
            }

        } else {
            if(YY==0x02)
            {
                encoder_position++;
            }else if(YY==0x01)
            {
                encoder_position--;
            }
            // The encoder has moved one position CW

        }
        prevA=A;
        prevB=B;







        // Toggle the edge detection configuration for PA5
                GPIO_PORTA_IEV_R ^= 0x20;
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
      vTaskDelay(10);
      interrupt_handler();
      home_LCD();
      send_encoder_position(encoder_position);


      }

}
