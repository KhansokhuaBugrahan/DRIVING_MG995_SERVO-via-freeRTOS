/*
 * servo.h
 *
 *  Created on: Jan 11, 2026
 *      Author: khans
 */

#ifndef SERVO_H_
#define SERVO_H_

#define    enableServo     		   1
#define    disableServo    		   0
#define    SLEW_RATE        	   10

#define    ADC_MID_POINT     	   2048
#define    ADC_MID_REAL            2000
#define    ADC_CENTER_DEAD_ZONE    10

#define    SERVO_MID_US   		   1500

typedef struct{

	int16_t previousPulseRead;
	int16_t emaFilteredPulse;

}Servo_Param;

uint16_t returnPulseMsServoX(uint16_t currentAdcVal, Servo_Param* servoX);


#endif /* SERVO_H_ */
