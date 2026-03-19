# DRIVING_MG995_SERVO-via-freeRTOS
The servo motor has ability to rotate  by a POT between 0 - 180 degree. Also, motor on/off option is available.

Board: STM32F407G-DISC1                              
  IDE: STM32CubeIDE                     
  
Note: if you do not know how to clear errors when integrating freeRTOS file into the project.
https://github.com/KhansokhuaBugrahan/LED_TOGGLE-via-freeRTOS                               
In the above link, you can follow the steps in readme file to solve issues.

You can include main.c from either joystick files or POT file in main folder.                                                                                              
Additionally, you need to include modified source files and header files  which are also located in main folder for multi-servo(two servo control)          
All multi servo files are common. You can also see DMA(direct memory access) usage in one of multi servo files.

Deadband width is 5us in the datasheet.                                  
That means servo controller inside the motor perceives  x <= 5us , pulses as a noise.                                              
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

Since POT's wiper is a bit problematic, I clamped the DR value of ADC at 3100.                                     
In this way, I removed noisy part of the POT. Now, it only mechanically rotates without acting on anything.                  


Click the icon  to redirect YouTube and watch the MG995 Servo POT video .                                           
[![Custom Thumbnail](https://github.com/KhansokhuaBugrahan/DRIVING_MG995_SERVO-via-freeRTOS/blob/main/gif.gif)](https://www.youtube.com/shorts/Kpw473yBjIc)







Click the icon below to redirect YouTube and watch the MG995 Servo JOYSTICK video .                                           
In the  second implementation, the servo motor is snapping back to the mid-point in idle.
[![Custom Thumbnail](https://github.com/KhansokhuaBugrahan/DRIVING_MG995_SERVO-via-freeRTOS/blob/main/gif.gif)](https://www.youtube.com/shorts/IurvvpIOO_4)


















When using joystick, in idle it gives output around 2000 DR val( mid point). So, we need to map  0-3100 raw ADC values into the 500 - 2500 us pulses by linearizing.
And this analytic geometry approach helps us to find a proper equation even though it seems easy.                
Below link, you can examine the equations and illustrations which is also located in SERVO_JOYSTICK folder.                                            
https://github.com/KhansokhuaBugrahan/DRIVING_MG995_SERVO-via-freeRTOS/blob/main/Src/main/SERVO_JOYSTICK_SNAPPING/ADCmapping.jpeg           
joystick datasheet --> https://naylampmechatronics.com/img/cms/Datasheets/000036%20-%20datasheet%20KY-023-Joy-IT.pdf 


                                                             
In the  third implementation, the servo motor runs at constant speed in entire operation.                                                   
When it comes back from the max range (lower side), it is quite smooth.                         
However, when it comes from upper side, it takes a few seconds to stabilize in mid point.                
Click the icon below to redirect YouTube and watch the MG995 Servo JOYSTICK full slew rate movement.                         
[![Custom Thumbnail](https://github.com/KhansokhuaBugrahan/DRIVING_MG995_SERVO-via-freeRTOS/blob/main/gif.gif)](https://youtube.com/shorts/tfo6ze2H3Y4)










In the  fourth implementation, the two servo motor runs at constant speed in entire operation.  
Click the icon below to redirect YouTube and watch the MG995 Multi(two) Servo JOYSTICK full slew rate movement.     
[![Custom Thumbnail](https://github.com/KhansokhuaBugrahan/DRIVING_MG995_SERVO-via-freeRTOS/blob/main/gif.gif)](https://www.youtube.com/shorts/u8_ZhK-d968)





In the  fifth implementation, the two servo motor is completely controlled by joystick.                                                         
In previous operations, we were using an external button to control the motors' state but now we use joystick's own switch.                                                  
The difference here, since the switch is active low, we trigger the interrupt in falling edge.                                                  
Additionally, you need to include the same  source and header files as we use for fourth multi servo operation.                                        
Click the icon below to redirect YouTube and watch the MULTI SERVO FULL JOYSTICK CONTROL.                                         
[![Custom Thumbnail](https://github.com/KhansokhuaBugrahan/DRIVING_MG995_SERVO-via-freeRTOS/blob/main/gif.gif)](https://youtube.com/shorts/9kMJpKzMdMA)











In the  sixth implementation, the two servo motor is not snapping back to mid-point anymore. 
All previus functions exist on this newest model except motor movement.                 
It has an EEPROM (AT24C32) and a voltage-drop detector circuit(based on LM393) which notifies EEPROM.            
Also, individual control on each servo is available.                          
It always maintain the previous position because of EEPROM save.                  
I power my board using PC via USB socket.                       
I have a voltage divider resistors on Vref and V+ on LM393, 5V input that comes from board's 5V pin.                
You can add 100nF cap to supply.                           

LM393 output is open collector in case of V+ > Vref that means we can have pull up resistor on output and trigger some GPIO.
The interesting thing here is I have tried many times to use LM393's output for triggering my GPIO pin.
However, it was not reliable at all. And accidently I wired my output to Vref than it started working perfectly. 
The current circuit uses Vref as output for triggering B1 pin which was configured to save servo positions. 

Also, you can adjust the servo movement speed by adjusting tuning parameter in servo.c file changing coefficient 200.

I share the circuit diagram and video of this implementation. main directory name MULTI_SERVO_SINGLE_CONTROL



![LM393 circuit](https://github.com/user-attachments/assets/6b52db9d-35c8-427d-80a8-c3c8e7d5ebeb)

[![Custom Thumbnail](https://github.com/KhansokhuaBugrahan/DRIVING_MG995_SERVO-via-freeRTOS/blob/main/gif.gif)](https://youtu.be/G3ARnHSSFyY)

