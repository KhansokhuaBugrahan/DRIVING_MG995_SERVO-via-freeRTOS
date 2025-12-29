# DRIVING_MG995_SERVO-via-freeRTOS
The servo motor has ability to rotate  by a POT between 0 - 180 degree. Also, motor on/off option is available.

Board: STM32F407G-DISC1                              
  IDE: STM32CubeIDE                     
  
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
It also stabilizes the servo movement. You can adjust this constant SLEW_RATE.    

We have to set interrupt priority for EXTI9_5.              
Priority levels 0–4 are reserved for system handlers used by FreeRTOS.     
As default EXTI9_5 priortiy is 0 (highest). We need to pull that down.        

You may consider 4 blinking leds debugger.

Click the icon to redirect YouTube and watch the MG995 Servo video.                                           
[![Custom Thumbnail](https://github.com/KhansokhuaBugrahan/DRIVING_MG995_SERVO-via-freeRTOS/blob/main/gif.gif)](https://www.youtube.com/shorts/Kpw473yBjIc)


