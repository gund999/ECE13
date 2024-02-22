/* Evan Draeger
 * 1/9/24
 * main.c for Lab0
 */

#include <stdio.h>
#include <stdlib.h>
#include <Board.h>
#include <ADC.h> // ADC pin use for reading POT
#include <pwm.h> // PWM pins for providing ref signal to audio amp
#include <buttons.h> // Allows use of buttons


/*******************************************************************
 To select which part of the code you would like to execute, uncomment
 the define that corresponds to the relevent section 
********************************************************************/

// Macros
#define TOP_R_PWM_PIN PWM_0 
#define BUTTON0 0b00000001
#define BUTTON1 0b00000010
#define BUTTON2 0b00000100
#define BUTTON3 0b00001000

#define HELLO_WORLD
// #define POT_TONES
// #define MUSIC

// Init Vals
uint32_t lastVal = 0;
unsigned int normalizedFreq = 100;

// Button Debouncing Event Checker (w/out interrupts)




// Main
int main(void) {
    // Inits clocks, serial port, and leds
    BOARD_Init();
    // Init ADC
    ADC_Init();
    // Init PWM
    PWM_Init();
    // Init Buttons for Music Mode
    BUTTONS_Init();
    // Set up PWM pin(pin 48 on I/O Shield)
    PWM_AddPin(TOP_R_PWM_PIN);
    // Set Pin 48(Top Right) to a Duty Cycle of 50% (should be volume?)
    PWM_SetDutyCycle(TOP_R_PWM_PIN, 5);
    #ifdef POT_TONES
    PWM_Start(TOP_R_PWM_PIN);
    #endif


    // Read pot
    ADC_Read(POT);
    while (TRUE) {
        
        // *********************************
        #ifdef HELLO_WORLD
        printf("Hewo World!\r\n");
        #endif
        // *********************************

        
        // ---------------------------------
        #ifdef POT_TONES
        // Read pot
        uint32_t potVal = ADC_Read(POT);
        // If pot has changed:
        if (abs(potVal - lastVal) > 100) {
            // printf("Pot Val: %lu\r\n", potVal);
            // printf("Last Val: %lu\r\n", lastVal);
            // printf("Difference: %lu\r\n", abs(potVal - lastVal));
            lastVal = (potVal+lastVal)/2;
            // Set Freq for PWM(100[Hz] - 100[kHz])
            normalizedFreq = ((4800*potVal/4100)+200);
            PWM_SetFrequency(normalizedFreq);
            printf("Freq: %u, Pred: %u\r\n", PWM_GetFrequency(), normalizedFreq); // b/c cant find 3V pin on I/O Shield
        }
        #endif
        // ---------------------------------


        // =================================
        #ifdef MUSIC
        
        // Read pot
        uint32_t potVal = ADC_Read(POT);
        // If pot has changed:




        if (abs(potVal - lastVal) > 100) { // You can adjust the pot val to change the sensitivity
            lastVal = (potVal+lastVal)/2;
            // Set Freq for PWM(100[Hz] - 100[kHz])
            normalizedFreq = ((4800*potVal/4100)+200);
            // PWM_SetFrequency(normalizedFreq);
            printf("Pot Val: %u, Freq: %u, Pred: %u\r\n", potVal, PWM_GetFrequency(), normalizedFreq); // b/c cant find 3V pin on I/O Shield
        }

        // Higher # button = Higher Freq w/ equally stepped dif in freq btwn each button
        // Note: would prob be better to have an event checker then when button state changes, then tones start or stop but whatever
        // printf("Button State: %u", buttons_state());
        if ((~buttons_state() & BUTTON0) == 0b0001) {
            PWM_SetFrequency(normalizedFreq);
            PWM_Start(TOP_R_PWM_PIN);
            // printf("button 0");
        } else if ((~buttons_state() & BUTTON1) == 0b0010) {
            PWM_SetFrequency(normalizedFreq+200);
            PWM_Start(TOP_R_PWM_PIN);
            // printf("button 1");
        } else if ((~buttons_state() & BUTTON2) == 0b0100) {
            PWM_SetFrequency(normalizedFreq+400);
            PWM_Start(TOP_R_PWM_PIN);
            // printf("button 2");
        } else if ((~buttons_state() & BUTTON3) == 0b1000) {
            PWM_SetFrequency(normalizedFreq+600);
            PWM_Start(TOP_R_PWM_PIN);
            // printf("button 3");
        } else {
            PWM_Stop(TOP_R_PWM_PIN);
            // printf("Fuck Sound");
        }
        #endif
        // =================================

    }
}