/**********************************************************************
  Filename: UW_eng.cpp
  Date: October 25, 2013
  File Version: 2.4

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
  2.4  Oct 25/13   Turn off ADC and ENC after each conversion; Make sure to
                   toggle ENC according to user guide
  2.3  Oct 16/13   Adjust motor direction to match wiring
  2.2  Sep 26/13   Add support for button on P1.3; code review for release;
  2.1  Jun 21/13   Removed motor controller reset pin; flipped forward and
                   reverse for motor to match wiring
  2.0  Jun  6/13   Bumper moved to P2.5 from P2.0
                   Added support for light bar of LEDs                   
  1.0  Apr 25/13   original release

**********************************************************************/
#ifndef _UW_ENG_CPP
#define _UW_ENG_CPP
#include <cmath>
#include "io430.h"
#include "UW_eng.h"

void initialize()
{
    // stop watch dog timer
    WDTCTL = WDTPW + WDTHOLD;
  
    // set CPU clock speed
    BCSCTL1 = _BCS_CLK;
    DCOCTL = _DCO_CLK;

    // set launchpad board LED and button pin directions
    P1DIR_bit.P0 = _OUTPUT;
    P1DIR_bit.P3 = _INPUT;
    P1DIR_bit.P6 = _OUTPUT;

    // turn on internal pull-up resistor for launchpad push button
    P1REN_bit.P3 = _HIGH;
    
    // configure for fuel cell car
    initMotorController();
    initADC();

    // set switch pin direction
    P2DIR_bit._P_SWITCH = _INPUT;
    
    // set LED light bar pin directions to output
    P2DIR |= _LED_MASK;
}

void wait10Usec(int t)
{
    for ( ; t>0; t--)
        __delay_cycles(_US_10_TICK);
}

void wait1Msec(int t)
{
    for ( ; t>0; t--)
        __delay_cycles(_MS_TICK);
}

void wait10Msec(int t)
{
    for ( ; t>0; t--)
        for (int i=0; i<10; i++)
            __delay_cycles(_MS_TICK);
}

void wait1Sec(int t)
{
    for ( ; t>0; t--)
        wait1Msec(1000);
}

void setMotor(int motorNum, int dirSpeed)
{
    // set direction
    if (motorNum == MOTOR_A)  // fix to match motor wiring
        dirSpeed *= -1;
    unsigned char direction = _FWD;
    if (dirSpeed < 0)
    {
        direction = _REV;
        dirSpeed *= -1;
    }

    // set speed (max for user is 100)
    if (dirSpeed > 100)
        dirSpeed = 100;
    unsigned char speed = dirSpeed * _MOTORMAX / 100;

    // send commands to motor controller
    unsigned char motor = _MOTORB;

    switch (motorNum)
    {
      case MOTOR_A:
          motor = _MOTORA;
      case MOTOR_B:  // also for MOTOR_A
          sendMotorChar(_START_BYTE);
          sendMotorChar(_CNTL_DEVICE);
          sendMotorChar(motor | direction);
          sendMotorChar(speed & 0x7F);  // ensure MSB is zero
          break;
    }
}

int getSensor(int sensorNum)
{
    int sensorValue = 0;

    if (sensorNum == BUMPER)
        sensorValue = P2IN_bit._P_SWITCH;

    else if (sensorNum == BUTTON)
        sensorValue = !P1IN_bit.P3;   // active low, so invert

    else if (sensorNum == REFLECT_1 || sensorNum == REFLECT_2)
    {
        int pinADC = _ADC_1;
        if (sensorNum == REFLECT_1)
            pinADC = _ADC_0;

        // select pin, binary output, no clock divide, SMCLK
        ADC10CTL1 = (pinADC << 12) | ADC10SSEL1 | ADC10SSEL0;
  
        // ADC is on, enabled, and conversion is started
        ADC10CTL0 = ADC10ON | ENC | ADC10SC;
  
        // poll - wait for conversion complete
        while (ADC10CTL1 & ADC10BUSY);

        // turn ADC off and disable conversion
        ADC10CTL0 = 0;
        
        // return 10-bit value
        sensorValue = _ADC10MAX - ADC10MEM;
    }

    return sensorValue;
}

void LEDoff(int led)
{
    if (led == RED_LED)
        P1OUT_bit.P0 = _LOW;
    else if (led == GREEN_LED)
        P1OUT_bit.P6 = _LOW;
}

void LEDon(int led)
{
    if (led == RED_LED)
        P1OUT_bit.P0 = _HIGH;
    else if (led == GREEN_LED)
        P1OUT_bit.P6 = _HIGH;
}

void LEDnum(int led)
{
    if (led >= _LED_MIN && led <= _LED_MAX)
    {
        unsigned char led_set = (unsigned char)led & _LED_MASK;
        P2OUT &= ~_LED_MASK;
        P2OUT |= led_set;
    }
}

void initADC()
{
    // ADC enable channels A4 and A5
    ADC10AE0 = (1 << _ADC_0) | (1 << _ADC_1);
  
    // leave ADC on, single conversion
//    ADC10CTL0 = ADC10ON;
}

void initMotorController()
{
    // set motor controller bits to be output
    P2DIR_bit._P_MOTOR = _OUTPUT;
//    P2DIR_bit._P_RESET_MOTOR = _OUTPUT;

/*    
    // reset motor controller - minimum 2us low on reset line
    P2OUT_bit._P_RESET_MOTOR = _LOW;
    wait1Msec(1);
    P2OUT_bit._P_RESET_MOTOR = _HIGH;
    wait1Msec(1);
*/

    // set serial communication line high
    P2OUT_bit._P_MOTOR = _HIGH;
    wait1Msec(1);
}

void sendMotorChar(unsigned char b)
{
    // start bit
    P2OUT_bit._P_MOTOR = _LOW;
    wait10Usec(_BIT_PERIOD);
  
    // data bits
    for (int i=0; i<_EIGHT_BITS; i++)
    {
        // volatile int bit = b & 0x01;
        P2OUT_bit._P_MOTOR = b & 0x01;
        b >>= 1;  // shift to next bit
        wait10Usec(_BIT_PERIOD);
    }
  
    // stop bit(s)
    for (int i=0; i<_TWO_STOP_BIT; i++)
    {
        P2OUT_bit._P_MOTOR = _HIGH;
        wait10Usec(_BIT_PERIOD);
    }
}


#endif
/**********************************************************************
  Copyright(c) 2012-2013 C.C.W. Hulls, P.Eng., C. Rennick, and G. Chan.
  Students, staff, and faculty members at the University of Waterloo
  are granted a non-exclusive right to copy, modify, or use this software
  for non-commercial teaching, learning, and research purposes provided
  the author(s) are acknowledged except for as noted below.

  First year engineering students at UW can copy, modify, or use this
  software as part of programming the MSP 430 fuel cell cars for
  course assignments and/or project without acknowledging the source.
**********************************************************************/
