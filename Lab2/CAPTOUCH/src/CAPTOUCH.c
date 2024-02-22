/**
 * @file    CAPTOUCH.c
 * @brief   Capacitive touch sensing module
 * @author  Evan Draeger
 * @date    February 1, 2024
 * @detail  This module uses the external interrupt peripheral to detect touch inputs.
 *          The peripheral is configured to generate an interrupt every rising edge 
 *          of pin PB5 (ENC_B) which means that the difference of two interrupts gives 
 *          you the period of the signal. Use the timers.h library for timing operations.
 * 
 *          To alleviate setup headaches the code for setting up the peripheral and the interrupt is below.
 */

#ifndef CAPTOUCH_H
#define CAPTOUCH_H

/*******************************************************************************
 * INCLUDES                                                                    *
 ******************************************************************************/
#include <Board.h>
#include <timers.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include <stm32f4xx_hal_tim.h>
#include <stdio.h>
#include <stdlib.h>
#include "timers.h"

/*******************************************************************************
 * MACROS                                                                      *
 ******************************************************************************/
// typedef enum { // Capacitor State machine states
//     FIRST_TIME,
//     SECOND_TIME
// } CapState;
#define MOVING_AVG_SIZE 200
#define HIGH_THRESHOLD 120
#define LOW_THRESHOLD 80

/*******************************************************************************
 * Init Vals                                                                   *
 ******************************************************************************/
// static CapState currentState = FIRST_TIME;
// static CapState nextState = FIRST_TIME;

static uint32_t firstTime = 0;  // Start time of pulse
static uint32_t nextTime = 0;   // Ending time of pulse
static uint32_t periodTime = 0; // Length of 1 period btwen rising edges
// static uint32_t lastPeriodTime = 0;
// static uint8_t timingFlag = 0; 
static uint32_t movingAverageVals[MOVING_AVG_SIZE] = {0};
static unsigned int threshold;


/**
 * @function    CAPTOUCH_Init(void)
 * @brief       This function initializes the module for use. Initialization is 
 *              done by opening and configuring timer 2, opening and configuring the GPIO pin
 *              and setting up the interrupt.
 * @return      None
 */
void CAPTOUCH_Init(void) {
    //Configure GPIO pin PB5 
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    // EXTI interrupt init
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    // the rest of the function goes here
    TIMER_Init();

}

// void CapMachine(void) {
//     // switch(currentState) {
//     //     // Take Rising Edge Time
//     //     case FIRST_TIME:
//     //         // 
//     //         break;
//     //     // ___|-----|___ middle high section of wave
//     //     case SECOND_TIME:
//     //         // Measure time at beginning and end of wave, compute difference
//     //         break;
//     // }
//     // if (nextState != currentState) {
//     //         currentState = nextState;
//     // }
//     // firstTime = nextTime;
//     // nextTime = TIMERS_GetMicroSeconds();
//     // periodTime = nextTime - firstTime;
//     // // if (abs(lastPeriodTime-periodTime) > 20) {
//     //     printf("First Time: %li, Second Time: %li, Period Time: %li\r\n", firstTime, nextTime, periodTime);
//     // }
// }


// external interrupt ISR for rising edge of pin PB5  
void EXTI9_5_IRQHandler(void) {
    // EXTI line interrupt detected 
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5); // clear interrupt flag
    // anything that needs to happen on rising edge of PB5 (ENC_B) = pin 40
    firstTime = nextTime; // Reset first time
    nextTime = TIMERS_GetMicroSeconds(); // Take current time
    periodTime = nextTime - firstTime; // Find difference
    }
}

/**
 * @function    CAPTOUCH_IsTouched(void)
 * @brief       Returns TRUE if finger is detected. Averaging of previous measurements 
 *              may occur within this function, however you are NOT allowed to do any I/O
 *              inside this function.
 * @return      TRUE or FALSE
 */
char CAPTOUCH_IsTouched(void) {
    // Moving Average, overwrite old values
    for (int i = 0; i < MOVING_AVG_SIZE - 1; ++i) {
        movingAverageVals[i] = movingAverageVals[i + 1];
    }
    // Assign the new value to the most recent position
    movingAverageVals[MOVING_AVG_SIZE - 1] = periodTime;
    uint32_t sum = 0;
    // Calculate the average
    for (int i = 0; i < MOVING_AVG_SIZE; ++i) {
        sum += movingAverageVals[i];
    }
    uint32_t average = sum / MOVING_AVG_SIZE;
    // Hysteresis with changing thesholds
    if (average > threshold) {
        // printf("Period: %li", average);
        threshold = LOW_THRESHOLD;
        return TRUE;
    }
    threshold = HIGH_THRESHOLD;
    return FALSE;
}

#endif  /* CAPTOUCH_H */