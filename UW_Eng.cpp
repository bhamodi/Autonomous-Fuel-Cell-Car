/**********************************************************************
  Filename: UW_eng.cpp
  Date: October 04, 2014
  File Version: 2.5

  A set of functions that will allow students to run the fuel cell
  car without knowing the microprocessor and interfacing details.

  Students need to call initialize() as their first statement.  It turns
  the watchdog timer off.  If they forget this statement, then the
  processor will reset fairly quickly and the program will appear not
  to run at all.

  LEDs on the light bar are driven by a decoder, so only one can be on
  at any given time.

  History
  Ver  Date        Comment
  2.5  Oct 04/14   Cleaned and formatted code.
  2.4  Oct 25/13   Turn off ADC and ENC after each conversion. Make sure to
                   toggle ENC according to user guide.
  2.3  Oct 16/13   Adjust motor direction to match wiring.
  2.2  Sep 26/13   Add support for button on P1.3.
  2.1  Jun 21/13   Removed motor controller reset pin. Flipped forward and
                   reverse for motor to match wiring.
  2.0  Jun  6/13   Bumper moved to P2.5 from P2.0.
                   Added support for light bar of LEDs.
  1.0  Apr 25/13   Original release.
**********************************************************************/
#ifndef _UW_ENG_CPP
#define _UW_ENG_CPP
#include <cmath>
#include "io430.h"
#include "UW_eng.h"

void initialize() {
    // Stop watch dog timer.
    WDTCTL = WDTPW + WDTHOLD;
  
    // Set CPU clock speed.
    BCSCTL1 = _BCS_CLK;
    DCOCTL = _DCO_CLK;

    // Set launchpad board LED and button pin directions.
    P1DIR_bit.P0 = _OUTPUT;
    P1DIR_bit.P3 = _INPUT;
    P1DIR_bit.P6 = _OUTPUT;

    // Turn on internal pull-up resistor for launchpad push button.
    P1REN_bit.P3 = _HIGH;
    
    // Configure for fuel cell car.
    initMotorController();
    initADC();

    // Set switch pin direction.
    P2DIR_bit._P_SWITCH = _INPUT;
    
    // Set LED light bar pin directions to output.
    P2DIR |= _LED_MASK;
}

void wait10Usec(int t) {
    for ( ; t>0; t--) {
        __delay_cycles(_US_10_TICK);
    }
}

void wait1Msec(int t) {
    for ( ; t>0; t--) {
        __delay_cycles(_MS_TICK);
    }
}

void wait10Msec(int t) {
    for ( ; t>0; t--) {
        for (int i=0; i<10; i++) {
            __delay_cycles(_MS_TICK);
        }
    }
}

void wait1Sec(int t) {
    for ( ; t>0; t--) {
        wait1Msec(1000);
    }
}

void setMotor(int motorNum, int dirSpeed) {
    // Set direction.
    if (motorNum == MOTOR_A) {
        dirSpeed *= -1;
    }
    unsigned char direction = _FWD;
    if (dirSpeed < 0) {
        direction = _REV;
        dirSpeed *= -1;
    }

    // Set speed (max for user is 100).
    if (dirSpeed > 100) {
        dirSpeed = 100;
    }
    unsigned char speed = dirSpeed * _MOTORMAX / 100;

    // Send commands to motor controller.
    unsigned char motor = _MOTORB;

    switch (motorNum) {
      case MOTOR_A:
          motor = _MOTORA;
      case MOTOR_B:  // Also for MOTOR_A.
          sendMotorChar(_START_BYTE);
          sendMotorChar(_CNTL_DEVICE);
          sendMotorChar(motor | direction);
          sendMotorChar(speed & 0x7F);  // Ensure MSB is zero.
          break;
    }
}

int getSensor(int sensorNum) {
    int sensorValue = 0;

    if (sensorNum == BUMPER) {
        sensorValue = P2IN_bit._P_SWITCH;
    } else if (sensorNum == BUTTON) {
        sensorValue = !P1IN_bit.P3; // Active low, so invert.
    } else if (sensorNum == REFLECT_1 || sensorNum == REFLECT_2) {
        int pinADC = _ADC_1;
        if (sensorNum == REFLECT_1) {
            pinADC = _ADC_0;
        }
        // Select pin, binary output, no clock divide, SMCLK.
        ADC10CTL1 = (pinADC << 12) | ADC10SSEL1 | ADC10SSEL0;
  
        // ADC is on, enabled, and conversion is started.
        ADC10CTL0 = ADC10ON | ENC | ADC10SC;
  
        // Poll - wait for conversion complete.
        while (ADC10CTL1 & ADC10BUSY);

        // Turn ADC off and disable conversion.
        ADC10CTL0 = 0;
        
        // Return 10-bit value.
        sensorValue = _ADC10MAX - ADC10MEM;
    }
    return sensorValue;
}

void LEDoff(int led) {
    if (led == RED_LED) {
        P1OUT_bit.P0 = _LOW;
    } else if (led == GREEN_LED) {
        P1OUT_bit.P6 = _LOW;
    }
}

void LEDon(int led) {
    if (led == RED_LED) {
        P1OUT_bit.P0 = _HIGH;
    } else if (led == GREEN_LED) {
        P1OUT_bit.P6 = _HIGH;
    }
}

void LEDnum(int led) {
    if (led >= _LED_MIN && led <= _LED_MAX) {
        unsigned char led_set = (unsigned char)led & _LED_MASK;
        P2OUT &= ~_LED_MASK;
        P2OUT |= led_set;
    }
}

void initADC() {
    // ADC enable channels A4 and A5.
    ADC10AE0 = (1 << _ADC_0) | (1 << _ADC_1);
}

void initMotorController() {
    // Set motor controller bits to be output
    P2DIR_bit._P_MOTOR = _OUTPUT;

    // Set serial communication line high.
    P2OUT_bit._P_MOTOR = _HIGH;
    wait1Msec(1);
}

void sendMotorChar(unsigned char b) {
    // Start bit.
    P2OUT_bit._P_MOTOR = _LOW;
    wait10Usec(_BIT_PERIOD);
  
    // Data bits.
    for (int i=0; i<_EIGHT_BITS; i++) {
        // volatile int bit = b & 0x01;
        P2OUT_bit._P_MOTOR = b & 0x01;
        b >>= 1;  // Shift to next bit.
        wait10Usec(_BIT_PERIOD);
    }
  
    // Stop bit(s).
    for (int i=0; i<_TWO_STOP_BIT; i++) {
        P2OUT_bit._P_MOTOR = _HIGH;
        wait10Usec(_BIT_PERIOD);
    }
}

#endif
