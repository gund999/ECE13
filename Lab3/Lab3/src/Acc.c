/* Evan Draeger
 * 1/16/24
 * main.c for Lab1
 */

#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
// #include <ADC.h> // ADC pin use for reading POT
// #include <pwm.h> // PWM pins for providing ref signal to audio amp
// #include <timers.h> // Use timers for keeping speaker on
// #include "PING.h"
// #include "CAPTOUCH.h"
#include "BNO055.h"
#include "timers.h"

/*******************************************************************
 To select which part of the code you would like to execute, uncomment
 the define that corresponds to the relevent section 
********************************************************************/

// Macros
#define MOVING_AVG_SIZE 5
// #define acc

#ifdef acc
// Init Vals
static uint32_t movingAverageX[MOVING_AVG_SIZE] = {0};
static uint32_t movingAverageY[MOVING_AVG_SIZE] = {0};
static uint32_t movingAverageZ[MOVING_AVG_SIZE] = {0};
// int ZReading;
// int lastZReading = 0;
uint32_t lastTime = 0;
uint32_t lastPrintTime = 0;
// Main
int main(void) {
    BOARD_Init();
    BNO055_Init();
    HAL_Delay(1000);
    TIMER_Init();
    HAL_Delay(1000);
    while(TRUE) {
        // ZReading = BNO055_ReadAccelZ();
        if ((TIMERS_GetMilliSeconds() - lastTime) > 20) {
            lastTime = TIMERS_GetMilliSeconds();
            for (int i = 0; i < MOVING_AVG_SIZE - 1; ++i) {
                movingAverageX[i] = movingAverageX[i + 1];
                movingAverageY[i] = movingAverageY[i + 1];
                movingAverageZ[i] = movingAverageZ[i + 1];
            }
            // Assign the new value to the most recent position
            movingAverageX[MOVING_AVG_SIZE - 1] = BNO055_ReadAccelX();
            movingAverageY[MOVING_AVG_SIZE - 1] = BNO055_ReadAccelY();
            movingAverageZ[MOVING_AVG_SIZE - 1] = BNO055_ReadAccelZ();
            

            
            
        }
        
        if ((TIMERS_GetMilliSeconds() - lastPrintTime) > 50) {
            lastPrintTime = TIMERS_GetMilliSeconds();
            // Calculate the X average
            int sum = 0;
            int average = 0;
            // double g_s = 0;
            for (int i = 0; i < MOVING_AVG_SIZE; ++i) {
                sum += movingAverageX[i];
            }
            average = sum / MOVING_AVG_SIZE;\
            // g_s = (average + 10) / 1000.0;
            // printf("%.2lf, ", g_s);
            printf("%i, ", average);
            // Calculate the Y average
            sum = 0;
            for (int i = 0; i < MOVING_AVG_SIZE; ++i) {
                sum += movingAverageY[i];
            }
            average = sum / MOVING_AVG_SIZE;
            // g_s = (average + 14.5) / 989.5 ;
            // printf("%.2lf, ", g_s);
            printf("%i, ", average);
            
            // Calculate the Z average
            sum = 0;
            for (int i = 0; i < MOVING_AVG_SIZE; ++i) {
                sum += movingAverageZ[i];
            }
            average = sum / MOVING_AVG_SIZE;
            // g_s = (average + 3.5)/ 999.5 ;
            // printf("%.2lf;\r\n ", g_s);
            printf("%i;\r\n ", average);
        }
    }
}

#endif