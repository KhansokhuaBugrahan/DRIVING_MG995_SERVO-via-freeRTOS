/*
 * servo.c
 *
 *  Created on: Jan 11, 2026
 *      Author: khans
 */
#include <stdint.h>
#include <stdlib.h>
#include "servo.h"

volatile char servoState;

uint16_t returnPulseUsServoX(uint16_t currentAdcVal, Servo_Param_t* servoX)
{

	 int16_t pulseWidth;

	 if(abs(currentAdcVal - ADC_MID_REAL) < ADC_CENTER_DEAD_ZONE)
	 {
		 pulseWidth = 1440;
	 }
	 else
	 {
			 if(currentAdcVal <= ADC_MID_POINT)
				   pulseWidth = 490 + (currentAdcVal >> 1);

			 else  pulseWidth = currentAdcVal - 610;
	 }


	 servoX->emaFilteredPulse = servoX->previousPulseRead + ((pulseWidth - servoX->previousPulseRead) >> 2);

	 int16_t delta = servoX->emaFilteredPulse - servoX->previousPulseRead;

	if(abs(delta) > 5)
	{
		 if(servoX->previousPulseRead <= 2500 && servoX->previousPulseRead >= 500)
		 {

			if(delta > SLEW_RATE)
			{
				servoX->previousPulseRead += SLEW_RATE;
				 return servoX->previousPulseRead;
			}
			else if(delta < -SLEW_RATE)
			{
				servoX->previousPulseRead -= SLEW_RATE;
				 return servoX->previousPulseRead;
			}
		 }
	}

	return servoX->previousPulseRead;

}
uint16_t returnPulseUsServoX2(uint16_t currentAdcVal, Servo_Param_t* servoX)
{
    int16_t joyDelta = currentAdcVal - ADC_MID_REAL;

    // Dead zone → HOLD
    if (abs(joyDelta) < ADC_CENTER_DEAD_ZONE)
        joyDelta = 0;

    // Convert joystick deflection to velocity
    int16_t step = joyDelta / 200;   // tuning parameter

    if (step > SLEW_RATE) step = SLEW_RATE;
    if (step < -SLEW_RATE) step = -SLEW_RATE;

    // Apply movement
    servoX->previousPulseRead += step;

    // Clamp limits
    if (servoX->previousPulseRead > 2500)
        servoX->previousPulseRead = 2500;
    if (servoX->previousPulseRead < 500)
        servoX->previousPulseRead = 500;

    return servoX->previousPulseRead;
}
