/*
    NOTE TO USER: Specify course number before runtime. All 3 courses are in this file.
*/

#include "UW_eng.h"

void main() {
    initialize();

    // Safety precaution - ensure car is not moving yet.
    setMotor(MOTOR_A, 0);
    setMotor(MOTOR_B, 0);
    
    // Wait for button press to start.
    while (getSensor(BUTTON) == 0);
    while (getSensor(BUTTON) == 1);
    
    // Modify this value for each course.
    int courseNumber = 1;
    drive(courseNumber);
}

/*
    Drive function has automation and PID control logic for all 3 courses.
*/
void drive(int courseNumber) {
    if (courseNumber == 1) {
        // Course #1 - Version 1
        int threshhold = 90;
          
        while (true) {
            setMotor(MOTOR_A, 100);
            setMotor(MOTOR_B, 100);
            
            if (getSensor(REFLECT_1) < threshhold) {
                setMotor(MOTOR_A, 0);
                while (getSensor(REFLECT_1) < threshhold);
            }
            
            if (getSensor(REFLECT_2) < threshhold) {
                setMotor(MOTOR_B, 0);
                while (getSensor(REFLECT_2) < threshhold);
            }
        }
    } else if (courseNumber == 2) {
        // Course #1 - Version 2 (PID CONTROL)
        int imotor_power = 100;
        
        float error = getSensor(REFLECT_2) - getSensor(REFLECT_1),
            error_prev = error,
            error_sum = 0,
            kp = 0.9,
            ki = 0.0,
            kd = 3,
            PID_power;
        
        while (true) {
            while (getSensor(BUTTON) == 0) {
                int A = getSensor(REFLECT_2),
                    B = getSensor(REFLECT_1);
                
                if (A > 150)
                    A = 150;
                if (B > 150)
                    B = 150;
                error = B - A;
            
                PID_power = kp * error + ki * error_sum + kd * (error - error_prev);
                
                setMotor(MOTOR_A, int(floor(imotor_power + PID_power)));
                setMotor(MOTOR_B, int(floor(imotor_power - PID_power)));
                wait1Msec(25);
                
                error_prev = error;
                error_sum += error;
            }
            setMotor(MOTOR_A, 0);
            setMotor(MOTOR_B, 0);
            while (getSensor(BUTTON) == 1) {}
            while (getSensor(BUTTON) == 0) {}
            while (getSensor(BUTTON) == 1) {}
        }
    } else if (courseNumber == 3) {
        // Course #2 and #3 (CURVE METHOD)
        int step = 100;
        
        while (true) {
            setMotor(MOTOR_A, 100);
            setMotor(MOTOR_B, step);
            
            if (getSensor(BUTTON) == 1) {
    	       LEDoff(RED_LED);
    	       step -= 1;
    	       while (getSensor(BUTTON) == 1);
    	       LEDon(RED_LED);
            }
            
            wait1Msec(80);
        }
    } else {
        // Invalid course number. Terminate.
        setMotor(MOTOR_A, 0);
        setMotor(MOTOR_B, 0);
    }
}

/*
    Function for testing left and right reflection sensor values on different surfaces.
*/
void testReflectionSensor(int &leftSensor, int &rightSensor) {
    leftSensor = getSensor(REFLECT_1);
    rightSensor = getSensor(REFLECT_2);
}
