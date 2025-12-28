# DRIVING_MG995_SERVO-via-freeRTOS
The servo motor has ability to rotate  by a POT between 0 - 180 degree. Also, motor on/off option is available.

Note: if you do not know how to clear errors when integrating freeRTOS file into the project.
https://github.com/KhansokhuaBugrahan/LED_TOGGLE-via-freeRTOS                               
In the above link, you can follow the steps in readme file to solve issues.


Deadband width is 5us in the datasheet. That means servo controller inside the motor perceives  x <= 5us , pulses as a noise.                               
We make sure our servo does not pull the current because of this constrain by ignoring  0 - 5us pulses.                        

EMA Filter, is a digital filter which cuts off the noises. In this filter, previous data is used to define new data with the current incoming data.             
new data = 0.875 * previous data + 0.125 * current incoming data. Previous data is always more dominant.                          
In this way, we only store previouse data to utilize in the next step.                                            

In both attempts, we aim to prevent motor from jittering.             

Lastly, defining slew-rate.  
We use this method  to obtain a constant speed in movement.                                            
In per cycle, the motor can only move as much as slew-rate.                                         
It also stabilizes the servo movement.
