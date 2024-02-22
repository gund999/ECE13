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
#include "QEI.h"
#include "pwm.h"
// #include "PING.h"

/*******************************************************************
 To select which part of the code you would like to execute, uncomment
 the define that corresponds to the relevent section 
********************************************************************/

// Macros
#define PWM_CR_PIN48 PWM_0
#define PWM_CG_PIN50 PWM_2
#define PWM_CB_PIN52 PWM_4

// If N Def For each Part
// #define QEI // Part 1
// #define PING // Part 2

#ifdef QEI
//Init QEI Vals
int DegRot = 0;
int AbsDeg = 0;

int R_val = 0;
int B_val = 0;
int G_val = 0;
// QEI Main
int main(void) {
    // Setup
    // Inits clocks, serial port, and leds
    BOARD_Init();
    // Init ADC
    // ADC_Init();
    // Init PWM
    PWM_Init();
    // Init Timers;
    TIMER_Init();
    // Set up PWM pin(pin 48 on I/O Shield)
    PWM_AddPin(PWM_CR_PIN48); // red
    PWM_AddPin(PWM_CG_PIN50); // green
    PWM_AddPin(PWM_CB_PIN52); // blue
    // Set Pin 48(Top Right) to a Duty Cycle of 5% (should be volume?)
    // PWM_SetDutyCycle(TOP_R_PWM_PIN, 5);
    PWM_Start(PWM_CR_PIN48);
    PWM_Start(PWM_CG_PIN50);
    PWM_Start(PWM_CB_PIN52);
    // Start Rotary Encoder Position Tracking
    QEI_Init();
    // int countedRot = QEI_GetPosition();
    
    
    printf("\rStart QEI\n");
    printf("Deg: %i\r\n", DegRot);

    while(TRUE) {
        if (DegRot != QEI_GetPosition()) {
            DegRot = QEI_GetPosition();
            printf("Deg: %i\r\n", DegRot);
        }
        if (abs(QEI_GetPosition()/15) == 0) {
            R_val = 0;
            B_val = 0;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);            
        } else if (abs(QEI_GetPosition()/15) == 1) {
            R_val = 0;
            B_val = 25;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 2) {
            R_val = 0;
            B_val = 50;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 3) {
            R_val = 0;
            B_val = 75;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 4) {
            R_val = 0;
            B_val = 100;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 5) {
            R_val = 0;
            B_val = 100;
            G_val = 75;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 6) {
            R_val = 0;
            B_val = 100;
            G_val = 50;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 7) {
            R_val = 0;
            B_val = 100;
            G_val = 25;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 8) {
            R_val = 0;
            B_val = 100;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 9) {
            R_val = 25;
            B_val = 100;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 10) {
            R_val = 50;
            B_val = 100;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 11) {
            R_val = 75;
            B_val = 100;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 12) {
            R_val = 100;
            B_val = 100;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 13) {
            R_val = 100;
            B_val = 75;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 14) {
            R_val = 100;
            B_val = 50;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 15) {
            R_val = 100;
            B_val = 25;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 16) {
            R_val = 100;
            B_val = 0;
            G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 17) {
            R_val = 100;
            B_val = 0;
            G_val = 25;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 18) {
            R_val = 100;
            B_val = 0;
            G_val = 50;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 19) {
            R_val = 100;
            B_val = 0;
            G_val = 75;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 20) {
            R_val = 100;
            B_val = 0;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 21) {
            R_val = 75;
            B_val = 0;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 22) {
            R_val = 50;
            B_val = 0;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else if (abs(QEI_GetPosition()/15) == 23) {
            R_val = 25;
            B_val = 0;
            G_val = 100;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        } else {
            // R_val = 0;
            // B_val = 0;
            // G_val = 0;
            PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
            PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
            PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        }
    
    }   
}
#endif

#ifdef PING
// Init Vals
unsigned int lastTOF = 0;

// Main
int main(void) {
    BOARD_Init();
    PING_Init();
    printf("\rPing Start:\n");
    while(TRUE) {
        if (lastTOF != PING_GetTimeofFlight()) {
            printf("TOF[us]%i \r\n:", PING_GetTimeofFlight());
            lastTOF = PING_GetTimeofFlight();
        }
    }
}
#endif

// printf("Count: %i\r\n", countedRot);
        // if (DegRot != QEI_GetPosition()) {
        //     DegRot = QEI_GetPosition();
        //     printf("Deg: %i\r\n", DegRot);
        // }

        // AbsDeg = abs(QEI_GetPosition());
        // // 0 - 45 Yellow to Orange
        // if (AbsDeg < 45) {
        //     R_val = 0;
        //     B_val = AbsDeg;
        //     G_val = 100;
        //     PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        // // 45 - 90 Orange to Red
        // } else if (AbsDeg < 90) {
        //     R_val = 0;
        //     B_val = AbsDeg+5;
        //     G_val = 100;
        //     PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        // // 90 - 135 Red to Pink
        // } else if (AbsDeg < 135) {
        //     R_val = 0;
        //     B_val = 100;
        //     G_val = (135-AbsDeg)*100/45;
        //     PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        // // 135 - 180 Pink to Purple
        // } else if (AbsDeg < 180) {
        //     R_val = 225-AbsDeg;
        //     B_val = 100;
        //     G_val = 0;
        //     PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        // // 180 - 225 Purple to Blue
        // } else if (AbsDeg < 225 ) {
        //     R_val = 100 - (225 - AbsDeg);
        //     B_val = 100;
        //     G_val = 0;
        //     PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        // // 225 - 270 Blue to Cyan
        // } else if (AbsDeg < 270) {
        //     R_val = 100;
        //     B_val = 100 - (270-AbsDeg); // 100 - 50
        //     G_val = 0;
        //     PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        // // 270 315 Cyan to Green
        // } else if (AbsDeg < 315) {
        //     R_val = (315-AbsDeg)*100/45;;
        //     B_val = 0;
        //     G_val = 100;
        //     PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        // // 315 - 360 Green to Yellow
        // } else if (AbsDeg <= 360) {
        //     R_val = 0;
        //     B_val = (360-AbsDeg);
        //     G_val = 100;
        //     PWM_SetDutyCycle(PWM_CR_PIN48, R_val);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, B_val);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, G_val);
        // } else {
        //     PWM_SetDutyCycle(PWM_CR_PIN48, 100);
        //     PWM_SetDutyCycle(PWM_CG_PIN50, 100);
        //     PWM_SetDutyCycle(PWM_CB_PIN52, 100);
        // }
        // PWM_Start(PWM_CR_PIN48);
        // PWM_Start(PWM_CG_PIN50);
        // PWM_Start(PWM_CB_PIN52);