/**
 * @file    PING.c
 * @brief   Ultrasonic Ping Sensor sensing module
 * @author  Evan Draeger
 * @date    January 30, 2024
 * @detail  This module uses the timer interrupt and extrenal trigger interrupt (EXTI) to drive the HC-SR04
 *          ultrasonic ranging sensor. 
 *          You will use GPIO pin PB8 (PWM_5) to toggle the 10 uS trigger pulse and GPIO pin PB5 (ENC_B) as an 
 *          external interrupt which is used to record the state of the echo pin (PB5). A timer-generated (TIM3)
 *          interrupt will be used to facillitate a state machine of your design. This interrupt is configured with 
 *          a 1:84 prescaler meaning it ticks at the frequency of the 1 Mhz. The period between interrupts
 *          is changed by modifying the ARR register. Please see the appendix on Timer interrupts. Lastly, you will 
 *          use the timers.h library to record timing information and calculate pulse durations.
 *
 *          To alleviate setup headaches the code for setting up the peripherals and interrupts is below.
 */

/*******************************************************************************
 * INCLUDES                                                                    *
 ******************************************************************************/
#include <stdint.h>
#include "Board.h"
#include "timers.h"
#include "PING.h"

/*******************************************************************************
 * MACROS                                                                      *
 ******************************************************************************/
#define TrigPin GPIO_PIN_8
#define currentTimerCount TIM3->CNT
#define sixtyMS 59999
#define tenUS 9
#define MOVING_AVG_SIZE 8

TIM_HandleTypeDef htim3;

/*******************************************************************************
 * Init Vars                                                                   *
 ******************************************************************************/
typedef enum { // QEI State machine states
    TRIGGER,
    WAIT
} PingState;

static PingState currentState = WAIT;
static PingState nextState = WAIT;
static uint8_t waitFlag = 0;
static uint8_t timerFlipFlag = 0;
static uint32_t outputStartTime = 0;
static uint32_t outputEndTime = 0;
static uint32_t timeCountDif = 0;
static uint32_t movingAverageVals[MOVING_AVG_SIZE] = {0};



/**
 * @function    PING_Init(void)
 * @brief       Sets up both the timer extrenal interrupt peripherals along with their
 *              Also configures PWM_5 as a GPIO output for the trigger pin. 
 * @return      SUCCESS or ERROR
 */
char PING_Init(void) {
    // init other libraries
    BOARD_Init();
    TIMER_Init();

    // this block initializes the GPIO output pin (PB8, PWM_5 on shield)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // this block inits the timer generated interrupt
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 83; // divide by 1 prescaler (84-1) = 1 Mhz tick, 1 us
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 0xFFFF; // MUST CHANGE. number of clock cycles between interrupts
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        return ERROR;
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
    {
        return ERROR;
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        return ERROR;
    }
    HAL_TIM_Base_Start_IT(&htim3); // start interrupt

    // this block inits the external pin-generted interrupt on any change of pin PB5 (ENC_B)
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    return SUCCESS;
}

// GPIO_PinState TrigPinState = HAL_GPIO_ReadPin(GPIOB, TrigPin); 
// HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
void PingMachine(void) {
    switch(currentState) {
        case TRIGGER:
            // Set period of interrupt timer to be 10us
            TIM3->ARR = tenUS;
            // Check to see if we finished triggger pulse, move to wait state
            GPIO_PinState TriggerPinState = HAL_GPIO_ReadPin(GPIOB, TrigPin);
            if (TriggerPinState == GPIO_PIN_SET) {
                nextState = WAIT;
            }
            // Set Trigger Pin High/Low 
            HAL_GPIO_TogglePin(GPIOB, TrigPin);
            break;
        case WAIT:
            // Set Period to be 60ms
            TIM3->ARR = sixtyMS;
            // We come around 2x, first interrupt flip flag, second time we set the next state
            if (waitFlag) {
                nextState = TRIGGER;
            }
            waitFlag = ~waitFlag;
            break;
    }
    if (nextState != currentState) {
            currentState = nextState;
    }
}


void EXTI9_5_IRQHandler(void) {
    // EXTI line interrupt detected 
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) { // PB5 pin 40 I/O shield
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5); // clear interrupt flag
        // anything that needs to happen when PB5 (ENC_B) changes state
        
        // time difference between 2 of these interupts to get output pulse length in time counts 
        if (!timerFlipFlag) { // determines whether we are measuring start or end time of pulse __s_|--|_e__ 
            outputStartTime = currentTimerCount;
        } else if (timerFlipFlag) {
            outputEndTime = currentTimerCount;
            timeCountDif = outputEndTime - outputStartTime;
        }
        timerFlipFlag = ~timerFlipFlag; // Flip Flag bc interrupt only called on output edges 
    }
}
// TIM3 ISR
void TIM3_IRQHandler(void) {
    if (__HAL_TIM_GET_IT_SOURCE(&htim3, TIM_IT_UPDATE) != RESET) {
    
    // state machine that deals with set period to do 10us and 60ms
    PingMachine();
    __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE); // clear interrupt flag
    }
 }


/**
 * @function    PING_GetDistance(void)
 * @brief       Returns the calculated distance in mm using the sensor model determined
 *              experimentally. 
 *              No I/O should be done in this function
 * @return      distance in mm
 */
unsigned int PING_GetDistance(void) {
    uint32_t timeOfFlight = PING_GetTimeofFlight();
    return (10 * timeOfFlight) / 59;
}

/**
 * @function    PING_GetTimeofFlight(void)
 * @brief       Returns the raw microsecond duration of the echo from the sensor.
 *              NO I/O should be done in this function.
 * @return      time of flight in uSec
 */
unsigned int PING_GetTimeofFlight(void) {
    // Update older readings of TOF
    for (int i = 0; i < MOVING_AVG_SIZE - 1; ++i) {
        movingAverageVals[i] = movingAverageVals[i + 1];
    }
    // Assign the new value to the most recent position
    movingAverageVals[MOVING_AVG_SIZE - 1] = timeCountDif;
    uint32_t sum = 0;
    for (int i = 0; i < MOVING_AVG_SIZE; ++i) {
        sum += movingAverageVals[i];
    }
    // Calculate the average
    uint32_t average = sum / MOVING_AVG_SIZE;
    
    return average;
}
