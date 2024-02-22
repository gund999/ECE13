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

/*******************************************************************
 To select which part of the code you would like to execute, uncomment
 the define that corresponds to the relevent section 
********************************************************************/

// Macros


// Init Vals
unsigned int lastTOF = 0;

// Main
int main(void) {
    BOARD_Init();
    PING_Init();
    PWM_Init();
    PWM_SetFrequency(100);
    PWM_SetDutyCycle(PWM_0, 5);
    PWM_AddPin(PWM_0); // TOp right PWM Pin
    printf("\rPing Start: \n");
    int frequency = 20;
    while(TRUE) {
        if (abs(lastTOF - PING_GetTimeofFlight()) > 20) {
            printf("TOF[us]: %i, Distance[mm]: %i, frequency[Hz]: %i \r\n", PING_GetTimeofFlight(), PING_GetDistance(), frequency);
            lastTOF = PING_GetTimeofFlight();
            if (PING_GetTimeofFlight() < 3800) {
                frequency = (100 + ((1000 - 100) * (PING_GetDistance() - 130) / (650 - 130)));
                PWM_SetFrequency(frequency);
                printf("freq: %i", frequency);
                PWM_Start(PWM_0);
            } else {
                PWM_Stop(PWM_0);
            }

        }
    }
}