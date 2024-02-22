/* 
 * @file    QEI.c
 * @brief   Quadrature Encoder sensing module
 * @author  Evan Draeger
 * @date    January 26, 2024
 * @detail  This module uses the external interrupt module to interface to a quadrature encoder. 
 *          The peripheral is configured to generate interrupts on the rising and falling edges of pins PB4
 *          and PB5 (ENC_A and ENC_B on the IO shield).
 *
 *          To alleviate setup headaches the code for setting up the peripheral and the interrupt ISR/IRQ is below.
 *          QEI_IRQ() gets called when ENC_A (PB4) or ENC_B (PB5) change state. This is where you will write your 
 *          state machine.
 */


/*******************************************************************************
 * INCLUDES                                                                    *
 ******************************************************************************/
#include <QEI.h>
#include <timers.h>

// testing w/ printf
#include <stdio.h>
#include <stdlib.h>
#include <Board.h>

/*******************************************************************************
 * Macros                                                                    *
 ******************************************************************************/
#define ENC_A GPIO_PIN_4
#define ENC_B GPIO_PIN_5
#define HIGH 1 // Define pin HIGH as logical 1
#define LOW 0 // Define pin  LOW  as logical 0
#define EDGE_CHANGE 1 // 
#define MAX_COUNT 24 // Max increments of encoder to roll over

// QEI State machine states
typedef enum { 
    ZERO_ZERO,
    ONE_ZERO,
    ONE_ONE,
    ZERO_ONE
} CountState;

/*******************************************************************************
 * VARS                                                                    *
 ******************************************************************************/
// Init Counter to 0
static int counter = 0;
// State Machine   
static CountState currentState; 
static CountState nextState;
static CountState prevState;
static uint16_t prev_Edge;
static uint16_t ENC_X;
static GPIO_PinState prevA = 1; // Store Prev vals for A and B to ensure correct state changes
static GPIO_PinState prevB = 1;
static uint8_t rolloverFlag = 0; // Flag goes up when we increment once(!) in 00 state such that we don't get extra weird increments 
// Interrupts
static uint32_t lastTimeA = 0;
static uint32_t lastTimeB = 0;


// static uint8_t prevA = LOW; // Init pin state to low, will be read in QEI_Init 
// static uint8_t prevB = LOW; // Init pin state 

// Helper Func: Check Pin High/Low -> Reset state machine, NOT counter
void resetState(void) {
    GPIO_PinState AState = HAL_GPIO_ReadPin(GPIOB, ENC_A); 
    GPIO_PinState BState = HAL_GPIO_ReadPin(GPIOB, ENC_B);
    if ((!AState) & (!BState)) {
        currentState = ZERO_ZERO;
        nextState = ZERO_ZERO;
        prevState = ZERO_ZERO;
        // printf("\rStart State: 00\n");
    } else if ((!AState) & (BState)) {
        currentState = ZERO_ONE;
        nextState = ZERO_ONE;
        prevState = ZERO_ONE;
        // printf("\rStart State: 01\n");
    } else if ((AState) & (BState)) {
        currentState = ONE_ONE;
        nextState = ONE_ONE;
        prevState = ONE_ONE;
        // printf("\rStart State: 11\n");
    } else if ((AState) & (!BState)) {
        currentState = ONE_ZERO;
        nextState = ONE_ZERO;
        prevState = ONE_ZERO;
        // printf("\rStart State: 10\n");
    }
}


/**
 * @function QEI_Init(void)
 * @param none
 * @brief  Enables the external GPIO interrupt, anything
 *         else that needs to be done to initialize the module. 
 * @return none
*/
void QEI_Init(void) {
    //Configure GPIO pins : PB4 PB5 
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ENC_A|ENC_B;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    // EXTI interrupt init
    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
    // the rest of the function goes here
    // prevA = HAL_GPIO_ReadPin(GPIOB, ENC_A); 
    // prevB = HAL_GPIO_ReadPin(GPIOB, ENC_B);
    TIMER_Init();
    resetState(); // set init state by reading pins
    // init time for interrupts
    lastTimeA = TIMERS_GetMicroSeconds(); 
    lastTimeB = TIMERS_GetMicroSeconds();
    
}


//state machine of your design
void QEI_IRQ(void) {
    // Store Current A,B pin state
    GPIO_PinState whatA = HAL_GPIO_ReadPin(GPIOB, ENC_A); 
    GPIO_PinState whatB = HAL_GPIO_ReadPin(GPIOB, ENC_B);        
    
    // Check to see if we need to reset the counter
    if (abs(counter) > MAX_COUNT) {
        counter = 0; // Could use reset function here, but why bother changing states?
    }

    // State Machine Switch           
    switch (currentState) {
        // First Edge
        // printf("we getting interruots\n");   
        case ZERO_ZERO:
            // printf("00\n");
            // Increment
            rolloverFlag = 1;
            if ((whatA) & (!whatB) & (!prevA) & (!prevB)) {
                nextState = ONE_ZERO;
                // counter += EDGE_CHANGE;
                // printf("ZERo Count:%f", counter);
            // Decrement
            } else if ((!whatA) & (whatB) & (!prevA) & (!prevB)) {
                nextState = ZERO_ONE;
                // counter -= EDGE_CHANGE;
            // No change
            } else {
                nextState = ZERO_ZERO;
            }
            break;

        // 2nd Edge
        case ONE_ZERO:
            // printf("10\n");
            // Increment
            if ((whatA) & (whatB) & (prevA) & (!prevB) & (rolloverFlag == 1)) {
                nextState = ONE_ONE;
                counter += EDGE_CHANGE;
                rolloverFlag = 0;
            // Decrement
            } else if ((!whatA) & (!whatB) & (prevA) & (!prevB)) {
                nextState = ZERO_ZERO;
                // counter -= EDGE_CHANGE;
            // No change
            } else {
                nextState = ONE_ZERO;
            }
            break;

        // 3rd Edge
        case ONE_ONE:
            // printf("11\n");
            // Increment
            if ((!whatA) & (whatB) & (prevA) & (prevB)) {
                nextState = ZERO_ONE;
                // counter += EDGE_CHANGE;
            // Decrement
            } else if ((whatA) & (!whatB) & (prevA) & (prevB)) {
                nextState = ONE_ZERO;
                // counter -= EDGE_CHANGE;
            // No change
            } else {
                nextState = ONE_ONE;
            }

            break;
        // 4th Edge
        case ZERO_ONE:
            // printf("01\n");
            // Increment
            if ((!whatA) & (!whatB) & (!prevA) & (prevB)) {
                nextState = ZERO_ZERO;
                // counter += EDGE_CHANGE;
            // Decrement
            } else if ((whatA) & (whatB) & (!prevA) & (prevB) & (rolloverFlag == 1)) {
                nextState = ONE_ONE;
                counter -= EDGE_CHANGE;
                rolloverFlag = 0;
            // No change
            } else {
                nextState = ZERO_ONE;
            }
            break;
    }
    prev_Edge = ENC_X; // vesitigal from checking what type of interrupt
    
    // If Init State, go to inc or dec and do that shit once more so "count" properly changes 
    if (nextState != currentState) {
        currentState = nextState;
        prevA = whatA;
        prevB = whatB;
    }
    // currentState = nextState;
    // printf("Count: %i, AB: %i%i\r\n\n", counter, whatA, whatB);
}


// external interrupt ISR for pin PB5 = B
void EXTI9_5_IRQHandler(void) {
    // EXTI line interrupt detected 
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5); // clear interrupt flag
        // printf("B interrupt\r\n");
        if ((TIMERS_GetMilliSeconds() - lastTimeB) > 2) {
            lastTimeB = TIMERS_GetMicroSeconds();
            ENC_X = ENC_B;
            // printf("B interrupt: %i%i\r\n", HAL_GPIO_ReadPin(GPIOB, ENC_A), HAL_GPIO_ReadPin(GPIOB, ENC_B));;
            // printf("");
            QEI_IRQ();
        }
    }
}
// external interrupt ISR for pin PB4 = A
void EXTI4_IRQHandler(void) {
    // EXTI line interrupt detected 
    if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4) != RESET) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4); // clear interrupt flag
        // printf("A interrupt\r\n");
        // Ensure 2ms delay, logically doesn't work, but code work. Assuming it's the same as no timer and all interrupts are accounted for
        if ((TIMERS_GetMilliSeconds() - lastTimeA) > 2) {
            lastTimeA = TIMERS_GetMicroSeconds(); // see? It's milli vs micro 
            // ENC_X = ENC_A;
            // printf("A interrupt: %i%i\r\n", HAL_GPIO_ReadPin(GPIOB, ENC_A), HAL_GPIO_ReadPin(GPIOB, ENC_B));
            QEI_IRQ();
        }
        
    }
}



/**
 * @function QEI_GetPosition(void) 
 * @param none
 * @brief This function returns the current position of the Quadrature Encoder in degrees.      
*/
int QEI_GetPosition(void) {
    // nextState = ZERO_ZERO;
    int degrees = (counter) * 15; // 15 deg per increment
    return degrees;
}

/**
 * @Function QEI_ResetPosition(void) 
 * @param  none
 * @return none
 * @brief  Resets the encoder such that it starts counting from 0.
*/
void QEI_ResetPosition() {
    counter = 0; // reset counter
    resetState(); // reset states
}

/* JUNK CODE I MIGHT NEED L8R

// ENC_A Rising/Falling Edge Event, A leads B
            if (ENC_X == ENC_A) {
                // Falling EDGE on A
                if (whatA == LOW) {
                    if (whatB == LOW) {
                        
                    } else if (whatB == HIGH) {
                        counter++;
                    }
                // Rising EDGE on A
                } else if (whatA == HIGH) {

                }

            // ENC_B Rising/Falling Edge Event
            } else if (ENC_X == ENC_B) {
                
            }
            



*/
