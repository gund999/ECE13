/* Evan Draeger
 * 1/16/24
 * main.c for Lab1
 */

#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <ADC.h> // ADC pin use for reading POT
#include <pwm.h> // PWM pins for providing ref signal to audio amp
#include <timers.h> // Use timers for keeping speaker on
#include "pwm.h"
#include "PING.h"
#include "CAPTOUCH.h"

/*******************************************************************
 To select which part of the code you would like to execute, uncomment
 the define that corresponds to the relevent section 
********************************************************************/

// Macros

// Init Vals


// Main
int main(void) {
    BOARD_Init();
    CAPTOUCH_Init();
    int touchFlag = 0;
    printf("\rStart Touch Sensor\n");
    while(TRUE) {
        if (CAPTOUCH_IsTouched()) {
            if (!touchFlag) {
                printf("Touch\r\n");
                touchFlag = 1;
            }
            
        } else {
            if (touchFlag) {
                printf("No Touch\r\n");
                touchFlag = 0;
            }
        }
    }
}
