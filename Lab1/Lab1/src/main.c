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

/*******************************************************************
 To select which part of the code you would like to execute, uncomment
 the define that corresponds to the relevent section 
********************************************************************/

// Macros
#define TOP_R_PWM_PIN PWM_0 

//Init Vals
uint32_t LastVal = 0;
uint32_t SecondndLastVal = 0;
uint32_t ThirdLastVal = 0;
uint32_t FourthLastVal = 0;
uint32_t AvgFlex = 0;
uint32_t LastChangedFlex = 0;
uint8_t FlexAngle = 0;

int normalizedFreq = 0;
int AvgFreq = 0;

uint32_t LastPressTime;

// Main
int main(void) {
    // Setup
    // Inits clocks, serial port, and leds
    BOARD_Init();
    // Init ADC
    ADC_Init();
    // Init PWM
    PWM_Init();
    // Init Timers;
    TIMER_Init();
    // Set up PWM pin(pin 48 on I/O Shield)
    PWM_AddPin(TOP_R_PWM_PIN);
    // Set Pin 48(Top Right) to a Duty Cycle of 5% (should be volume?)
    PWM_SetDutyCycle(TOP_R_PWM_PIN, 5);
    // Read first flexVal & fill moving average so not too many reads detected
    uint32_t flexVal = ADC_Read(ADC_0); // Pin 6 I/0
    // Filling Up Flex Moving Avg Vals
    LastVal = flexVal;
    SecondndLastVal = LastVal;
    ThirdLastVal = SecondndLastVal;
    FourthLastVal = ThirdLastVal;
    
    // Start PWM
    PWM_SetFrequency(normalizedFreq);     
    PWM_Start(TOP_R_PWM_PIN);           
    

    // Main Loop
    while (TRUE) {
        // Flex ADC Reading Moving Average
        flexVal = ADC_Read(ADC_0); // Pin 6 I/O shield
        AvgFlex = (SecondndLastVal + LastVal + flexVal)/3;
        FourthLastVal = ThirdLastVal;
        ThirdLastVal = SecondndLastVal;
        SecondndLastVal = LastVal;
        LastVal = flexVal;
        // printf("Flex Val: %li, Avg Flex: %li \r\n", flexVal, AvgFlex);
        
        // Detects difference in Flex Reading
        if (abs(AvgFlex - LastChangedFlex) > 20) {
            // printf("Flex Val: %li, Avg Flex: %li, Last Changed Avg: %li \r\n", flexVal/10, AvgFlex/10, LastChangedFlex/10);
            LastChangedFlex = AvgFlex;
            uint16_t FixedFlex = LastChangedFlex/10;
            // Binding Flex Angle to a Set Range
            if (FixedFlex > 240) {
                // printf("Too Big, ");
                FlexAngle = (1910000000 + (13200000*240) + (425*240*240*240) - (189000*240*240))/(10000000);
                
            } else if (FixedFlex < 96) {
                // printf("Too Small, ");
                FlexAngle = (1910000000 + (13200000*96) + (425*96*96*96) - (189000*96*96))/(10000000);
            } else {
                // printf("Just Right, ");
                // FlexAngle = 29900 - (126*FixedFlex);
                FlexAngle = (1910000000 + (13200000*FixedFlex) + (425*FixedFlex*FixedFlex*FixedFlex) - (189000*FixedFlex*FixedFlex))/(10000000); 
            
            }
            // Normalize Flex Angles from Frequency Range 500-1000 [Hz]
            normalizedFreq = (((FlexAngle)*10000/(180))*(1000-500))/10000 + 500;
            
            // Smooth Transitioning of New Freq to Output
            if ((AvgFreq - normalizedFreq) >= 3) {  
                AvgFreq -= 2;
            } else if ((normalizedFreq - AvgFreq) >= 3) {
                AvgFreq += 2;
            }


            // Print vals to check, and Change Freq in Output
            printf("Avg Freq: %u, Normal Freq: %u\r\n", AvgFreq, normalizedFreq);
            // PWM_SetFrequency(AvgFreq); 
            // PWM_Start(TOP_R_PWM_PIN);
        }
    // Piezo Sensor Hit Checker
    // printf("Piezo Val: %u\r\n", ADC_Read(ADC_1));
    // Hit Deteced? 
    if (ADC_Read(ADC_1) > 500) { // Pin 7 I/O shield
        LastPressTime = TIMERS_GetMilliSeconds(); // Reset timer
        PWM_SetFrequency(AvgFreq); 
        PWM_Start(TOP_R_PWM_PIN); // Start Speaker
        // printf("on");
    }
    // Time Elapsed? Stop Speaker
    if ((TIMERS_GetMilliSeconds() - LastPressTime) > 500) {
        // printf("off");
        PWM_Stop(TOP_R_PWM_PIN);
    }
    }    
}