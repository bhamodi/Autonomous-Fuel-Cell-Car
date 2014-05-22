/*
NOTE TO THE USER: Comment out all of the code except the code for the course
that you want to race on. All 3 courses are in this file.
*/

#include "UW_eng.h"

void main()
{
    initialize();
    
    /* REFLECTION SENSOR TESTING CODE
    while (true)
	{
      int a = getSensor(REFLECT_1);
      int b = getSensor(REFLECT_2);
      int c = a;
      c = b;
    }
   */
   
    setMotor(MOTOR_A, 0);
    setMotor(MOTOR_B, 0);
      
    while (getSensor(BUTTON) == 0);
    while (getSensor(BUTTON) == 1);
    
    // Course #1 - Version 1
    int threshhold = 90;
      
    while (true){
          
        setMotor(MOTOR_A, 100);
        setMotor(MOTOR_B, 100);
        
        if (getSensor(REFLECT_1) < threshhold){
            setMotor(MOTOR_A, 0);
            while (getSensor(REFLECT_1) < threshhold);
        }
        
        if (getSensor(REFLECT_2) < threshhold){
            setMotor(MOTOR_B, 0);
            while (getSensor(REFLECT_2) < threshhold);
        }
    }
      
    // Course #1 - Version 2 (PID CONTROL)
    int imotor_power = 100; //
    
    float error = getSensor(REFLECT_2) - getSensor(REFLECT_1),
		  error_prev = error,
		  error_sum = 0,
          kp = 0.9,
		  ki = 0.0,
		  kd = 3,
		  PID_power;
  
    while (true){
      
        while (getSensor(BUTTON) == 0){
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
        while (getSensor(BUTTON) == 1){}
        while (getSensor(BUTTON) == 0){}
        while (getSensor(BUTTON) == 1){}
    }
    
    // Course #2 and #3 (CURVE TOWARDS THE EXIT METHOD)
    int step = 100;
    
    while (true){
        setMotor(MOTOR_A, 100);
        setMotor(MOTOR_B, step);
        
        if (getSensor(BUTTON) == 1)
		{
	       LEDoff(RED_LED);
	       step -= 1;
	       while (getSensor(BUTTON) == 1);
	       LEDon(RED_LED);
        }
         
        wait1Msec(80);    
    }
}
