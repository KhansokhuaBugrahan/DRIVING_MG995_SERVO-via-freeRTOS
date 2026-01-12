/*
 * servo.c
 *
 *  Created on: Jan 11, 2026
 *      Author: khans
 */
#include <stdint.h>
#include <stdlib.h>
#include "servo.h"



uint16_t returnPulseMsServoX(uint16_t currentAdcVal, Servo_Param* servoX)
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
