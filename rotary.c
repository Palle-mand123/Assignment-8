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

void rotary_task(void *pvParameters) {
  init_rotary();
  const TickType_t xDelay = 1 / portTICK_PERIOD_MS;
  static int prevA=0;
  static int prevB=0;
  static int A ;
  static int B ;
  while(1)
      {
          vTaskDelay(xDelay);

          // Read the current state of pin PA5
              if ((0b00100000) & (GPIO_PORTA_DATA_R)) {
                      A = 1;
                  } else {
                      A = 0;
                  }
          // Read the current state of pin PA6
              if ((0b01000000) & (GPIO_PORTA_DATA_R)) {
                      B = 1;
                  } else {
                      B = 0;
                  }

                  int AB = (A << 1) | B;
                  int prevAB = (prevA << 1)| prevB;
                  INT8U YY = AB ^ prevAB;

                  if(AB==prevAB)
                  {}
                  else if(A==B) {
                  if(YY==0x01)
                  {
                      encoder_position++;
                  }
                  if(YY==0x02)
                  {
                      encoder_position--;
                  }
                  home_LCD();
                  send_encoder_position(encoder_position/2);

              } else {
                  if(YY==0x02)
                  {
                      encoder_position++;
                  }else if(YY==0x01)
                  {
                      encoder_position--;
                  }
                  home_LCD();
                  send_encoder_position(encoder_position/2);

              }

              prevA=A;
              prevB=B;



      }
}
