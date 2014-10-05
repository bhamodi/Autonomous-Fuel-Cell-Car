/**********************************************************************
  Filename: UW_eng.h
  Date: October 04, 2014
  File Version: 2.5

  Constants and function prototypes for fuel cell car based on
  MSP 430 G2553 chip.

  P1.0 - RED LED.
  P1.3 - MSP 430 launchpad push button.
  P1.4 - ADC 4 connected to reflectance sensor.
  P1.5 - ADC 5 connected to reflectance sensor.
  P1.6 - GREEN LED.
  P2.2, P2.1, P2.0 - Light bar LED to light (before decode), MSB is P2.2.
  P2.3 - Output connected to motor reset pin (no longer connected as of v2.1).
  P2.4 - Output connected to motor serial pin.
  P2.5 - Input connected to bumper microswitch.

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
#ifndef _UW_ENG_H
#define _UW_ENG_H
#include <cmath>
#include "io430.h"

// Definitions and prototypes for functions.
#define MOTOR_A 0
#define MOTOR_B 1
#define RED_LED 2
#define GREEN_LED 3
#define BUMPER 4
#define REFLECT_1 5
#define REFLECT_2 6
#define BUTTON 7

// MSP 430 configuration and motor controller initialization.
void initialize();

// Timing function prototypes - timing is partially accurate.
void wait10Usec(int t);
void wait1Msec(int t);
void wait10Msec(int t);
void wait1Sec(int t);

// Motor function.
void setMotor(int motorNum, int speed);

// Get current value of sensor.
int getSensor(int sensor);

// LED functions.
void LEDoff(int led);
void LEDon(int led);
void LEDnum(int led);

// Port directions.
#define _INPUT (unsigned char)0
#define _OUTPUT (unsigned char)1

// Signal definitions.
#define _LOW (unsigned char)0
#define _HIGH (unsigned char)1

// Clock choices for 16MHz CPU clock.
#define _BCS_CLK  CALBC1_1MHZ
#define _DCO_CLK  CALDCO_1MHZ

// Function loop consumes 2 cycles plus overhead.
#define _US_10_TICK  6L
#define _MS_TICK  996L

// MSP 430 pin definitions.
#define _ADC_0 4
#define _ADC_1 5
#define _LED_SHIFT 0
#define _LED_MASK (unsigned char)0x07
#define _P_MOTOR P4
#define _P_SWITCH P5

// Motor communication definitions.
#define _MOTORA (unsigned char)0x00
#define _MOTORB (unsigned char)0x02
#define _FWD (unsigned char)0x00
#define _REV (unsigned char)0x01
#define _START_BYTE (unsigned char)0x80
#define _CNTL_DEVICE (unsigned char)0x00

// Motor communication settings: 9600 baud, 8 bits, no parity, 1 stop bit.
#define _BIT_PERIOD 10
#define _EIGHT_BITS 8
#define _SEVEN_BITS 7
#define _ONE_STOP_BIT 1
#define _TWO_STOP_BIT 2

// Reflectance sensor maximum setting for sensor and motor.
#define _ADC10MAX 1023
#define _MOTORMAX 255

// Valid LED numbers.
#define _LED_MIN 0
#define _LED_MAX 7

// Motor functions.
void initADC();
void initMotorController();
void sendMotorChar(unsigned char b);

#endif
