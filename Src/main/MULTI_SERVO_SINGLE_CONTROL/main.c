/**
 ******************************************************************************
  file           : main.c
  author         : Khanshokhuo Bugrahan
  brief          : Main program body
 ******************************************************************************

 */

#include <stdint.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "gpio.h"
#include "timer.h"
#include "i2c.h"
#include "adc.h"
#include "servo.h"
#include "dma.h"
#include "EEPROM.h"


SemaphoreHandle_t semEnableServo; // created a semaphore

volatile uint8_t servoPos[2];

static Servo_Param_t servo1 = {1440, 1440};
static Servo_Param_t servo2 = {1440, 1440};

static TickType_t lastExtiTick = 0;


void vMyTask1BlinkLeds(void* pvTask1);
void vMyTask2EnableServo(void* pvTask3);
void vMyTask3GeneratePulse(void* pvTask2);
void vPowerFailTask(void *pv);


void vMyTask1BlinkLeds(void* pvTask1 )
{
	uint8_t ledColor = *(uint8_t*)(pvTask1);

		while(1)
		{

			gpioTogglePin(GPIOD, ledColor);
				vTaskDelay(pdMS_TO_TICKS(500));
		}
}
void vMyTask2EnableServo(void* pvTask2 )
{
	while(1)
	{
		xSemaphoreTake(semEnableServo,portMAX_DELAY);
		switch (servoState)
		{
			case SERVO_STATE_OFF: TIMER3_PWM_ENABLE();
								  servoState = SERVO_STATE_1;
								  break;
			case SERVO_STATE_1:   servoState = SERVO_STATE_2;
								  break;
			case SERVO_STATE_2:   TIMER3_PWM_DISABLE();
								  servoState = SERVO_STATE_OFF;
								  break;
			default:
			break;
		}

	}
}
void vMyTask3GeneratePulse(void* pvTask3 )
{


	while(1)
	{

		if(TIMER3_ENABLED())
		{
			 uint16_t buffer1 = dmaBuffer[0];
			 if(buffer1 > 3100) buffer1 = 3100;

			 uint16_t buffer2 = dmaBuffer[1];
		     if(buffer2 > 3100) buffer2 = 3100;

			 uint16_t pulseServo;
			switch (servoState)
			{
				case SERVO_STATE_1: pulseServo = returnPulseUsServoX2(buffer1,&servo1);
			                        TIMER3->TIMx_CCR1 = pulseServo;
								    break;

				case SERVO_STATE_2: pulseServo = returnPulseUsServoX2(buffer2,&servo2);
								    TIMER3->TIMx_CCR3 = pulseServo;
								    break;
						 default:   break;
			}

		}
			   vTaskDelay(pdMS_TO_TICKS(20));
	}

}



void EXTI1_IRQHandler(void)
{
    if(EXTI->PR & (1u << 1))
    {
        EXTI_SET_PR_1();

       saveServoPos();
    }
}
void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR & (1u << 5))
	{

		EXTI_SET_PR_5(); // clear pending register
		TickType_t now = xTaskGetTickCountFromISR();

		if ((now - lastExtiTick) > pdMS_TO_TICKS(500)) // 500 ms debounce
		{
			lastExtiTick = now;
			BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
			xSemaphoreGiveFromISR(semEnableServo, &pxHigherPriorityTaskWoken );
			portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
		}
	}
}

int main(void) {

	static uint8_t leds [4] = {LED_GREEN, LED_ORANGE, LED_RED, LED_BLUE};

	       GPIO_Handle_t orangeLed = {
										.PORTNAME = GPIOD,
										.PINCONF.PIN = LED_ORANGE,
										.PINCONF.MODE = GPIO_MODE_OUTPUT,
										.PINCONF.OTYPE = GPIO_OTYPE_PP,
										.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
										.PINCONF.PUPD = GPIO_PUPD_PU,
										.PINCONF.AF = AFNO
							         };
	          GPIO_Handle_t redLed = {
	        		  	  	  	  	    .PORTNAME = GPIOD,
										.PINCONF.PIN = LED_RED,
										.PINCONF.MODE = GPIO_MODE_OUTPUT,
										.PINCONF.OTYPE = GPIO_OTYPE_PP,
										.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
										.PINCONF.PUPD = GPIO_PUPD_PU,
										.PINCONF.AF = AFNO
									 };
		    GPIO_Handle_t blueLed = {
		    						    .PORTNAME = GPIOD,
										.PINCONF.PIN = LED_BLUE,
										.PINCONF.MODE = GPIO_MODE_OUTPUT,
										.PINCONF.OTYPE = GPIO_OTYPE_PP,
										.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
										.PINCONF.PUPD = GPIO_PUPD_PU,
										.PINCONF.AF = AFNO
									  };
		    GPIO_Handle_t greenLed = {
		    							.PORTNAME = GPIOD,
										.PINCONF.PIN = LED_GREEN,
										.PINCONF.MODE = GPIO_MODE_OUTPUT,
										.PINCONF.OTYPE = GPIO_OTYPE_PP,
										.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
										.PINCONF.PUPD = GPIO_PUPD_PU,
										.PINCONF.AF = AFNO
									  };

	              gpioInit(&orangeLed);
	              gpioInit(&redLed);
	              gpioInit(&blueLed);
	              gpioInit(&greenLed);

		 GPIO_Handle_t I2C1_SCL_PB6 = { .PORTNAME = GPIOB,
	              						.PINCONF.PIN = GPIO_PIN_6,
	              						.PINCONF.MODE = GPIO_MODE_ALTARNATE,
	              						.PINCONF.OTYPE = GPIO_OTYPE_OD,
	              						.PINCONF.OSPEED = GPIO_OSPEED_VHIGH,
	              						.PINCONF.PUPD = GPIO_PUPD_PU,
	              						.PINCONF.AF = AF4
	              					   };
		GPIO_Handle_t I2C1_SDA_PB7 = {  .PORTNAME = GPIOB,
	              						.PINCONF.PIN = GPIO_PIN_7,
	              						.PINCONF.MODE = GPIO_MODE_ALTARNATE,
	              						.PINCONF.OTYPE = GPIO_OTYPE_OD,
	              						.PINCONF.OSPEED = GPIO_OSPEED_VHIGH,
	              						.PINCONF.PUPD = GPIO_PUPD_PU,
	              						.PINCONF.AF = AF4
	              					  };

					gpioInit(&I2C1_SCL_PB6);
					gpioInit(&I2C1_SDA_PB7);

	              GPIO_Handle_t ADC_DIMMER_POT_PA0 = {  .PORTNAME = GPIOA,
														.PINCONF.PIN = GPIO_PIN_0,
														.PINCONF.MODE = GPIO_MODE_ANALOG,
														.PINCONF.OTYPE = GPIO_OTYPE_PP,
														.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
														.PINCONF.PUPD = GPIO_PUPD_NO,
														.PINCONF.AF = AFNO
													 };
	              GPIO_Handle_t ADC_DIMMER_POT_PA1 = {  .PORTNAME = GPIOA,
														.PINCONF.PIN = GPIO_PIN_1,
														.PINCONF.MODE = GPIO_MODE_ANALOG,
														.PINCONF.OTYPE = GPIO_OTYPE_PP,
														.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
														.PINCONF.PUPD = GPIO_PUPD_NO,
														.PINCONF.AF = AFNO
													 };
				   GPIO_Handle_t PWM_TIMER3_CH1_PB4 = {  .PORTNAME = GPIOB,  // use as output to generate wave
														.PINCONF.PIN = GPIO_PIN_4,
														.PINCONF.MODE = GPIO_MODE_ALTARNATE,
														.PINCONF.OTYPE = GPIO_OTYPE_PP,
														.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
														.PINCONF.PUPD = GPIO_PUPD_PU,
														.PINCONF.AF = AF2
													  };

				   GPIO_Handle_t PWM_TIMER3_CH2_PB0 = {  .PORTNAME = GPIOB,  // use as output to generate wave
														.PINCONF.PIN = GPIO_PIN_0,
														.PINCONF.MODE = GPIO_MODE_ALTARNATE,
														.PINCONF.OTYPE = GPIO_OTYPE_PP,
														.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
														.PINCONF.PUPD = GPIO_PUPD_PU,
														.PINCONF.AF = AF2
													  };

				   GPIO_Handle_t EXTI5_PB5_TRIGGER = {  .PORTNAME = GPIOB,  // use as trigger input
														.PINCONF.PIN = GPIO_PIN_5,
														.PINCONF.MODE = GPIO_MODE_INT_FALLING,
														.PINCONF.OTYPE = GPIO_OTYPE_PP,
														.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
														.PINCONF.PUPD = GPIO_PUPD_NO,
														.PINCONF.AF = GPIO_AF_NO
													  };


				   GPIO_Handle_t EXTI1_PB1_TRIGGER = {  .PORTNAME = GPIOB,  // use as trigger input
														.PINCONF.PIN = GPIO_PIN_1,
														.PINCONF.MODE = GPIO_MODE_INT_FALLING,
														.PINCONF.OTYPE = GPIO_OTYPE_PP,
														.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
														.PINCONF.PUPD = GPIO_PUPD_NO,
														.PINCONF.AF = GPIO_AF_NO
													  };
				    gpioInit(&PWM_TIMER3_CH1_PB4);
				    gpioInit(&PWM_TIMER3_CH2_PB0);
					gpioInit(&ADC_DIMMER_POT_PA0);
					gpioInit(&ADC_DIMMER_POT_PA1);


				   nvicSetInterruptPriority(IRQN_EXTI1, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
			  	   nvicSetInterruptPriority(IRQN_EXTI9_5, 6);
				   nvicInterruptEnable(IRQN_EXTI1);
				   nvicInterruptEnable(IRQN_EXTI9_5);
				   gpioInit(&EXTI5_PB5_TRIGGER);
				   gpioInit(&EXTI1_PB1_TRIGGER);


				   i2cInit();



				   timerxConfig(TIMER3, 16, 20000); // 1MHz --> 1us tick = 20ms 50Hz
				   timerxPeripheralEnable(TIMER3);

				   timer3PwmEnable(1440);


				   adc1Configuration(ADC_RESOLUTION_12, ADC_MODE_CONTINUOUS_CONV, ADC_CHANNEL_0, ADC_SAMPLING_144_CYCLE);
				   adc1Configuration(ADC_RESOLUTION_12, ADC_MODE_CONTINUOUS_CONV, ADC_CHANNEL_1, ADC_SAMPLING_144_CYCLE);
				   adc1ChannelSequence(ADC_CHANNEL_0, ADC_CHANNEL_SEQUENCE_1);
				   adc1ChannelSequence(ADC_CHANNEL_1, ADC_CHANNEL_SEQUENCE_2);

					ADC1_SCAN_MODE_ENABLE();
					adc1SequenceLength(ADC_CONVERSION_LENGTH_2);
					adc1Init();

					dma2Adc1Start();

					servoState = SERVO_STATE_OFF;

	 TaskHandle_t myTask1Handler = NULL;  //only need for if we want to manage the task after initializing

	 semEnableServo = xSemaphoreCreateBinary(); // handle semaphore whether as binary( active or  inactive)
	 xSemaphoreTake(semEnableServo, 0);




     xTaskCreate(vMyTask1BlinkLeds, "DEBUGLED1", 128, &leds[GREEN], 1, &myTask1Handler);
     xTaskCreate(vMyTask1BlinkLeds, "DEBUGLED2", 128, &leds[ORANGE], 1, &myTask1Handler);
     xTaskCreate(vMyTask1BlinkLeds, "DEBUGLED3", 128, &leds[RED], 1, &myTask1Handler);
     xTaskCreate(vMyTask1BlinkLeds, "DEBUGLED4", 128, &leds[BLUE], 1, &myTask1Handler);

     xTaskCreate(vMyTask3GeneratePulse, "DEBUGADC", 128, NULL, 2, &myTask1Handler);
     xTaskCreate(vMyTask2EnableServo, "DEBUGPWM", 128, NULL, 3, &myTask1Handler);

	   servoPos[0] = eepromReadfromAddress(SERVO_ADDRESS_1); // servo adreslerini macroya ata
	   servoPos[1] = eepromReadfromAddress(SERVO_ADDRESS_2);

	   if( servoPos[0] <= 250)
	   {
		   int16_t pulse = 500 + (servoPos[0] * 8);
		   servo1.previousPulseRead = pulse;
		   servo1.emaFilteredPulse = pulse;
		   TIMER3->TIMx_CCR1 = pulse;
	   }
	   else
	   {
		   servo1.previousPulseRead = 1440;
		   servo1.emaFilteredPulse = 1440;
	   }


	   if(servoPos[1] <= 250)
	   {
		   int16_t pulse = 500 + (servoPos[1] * 8);
		   servo2.previousPulseRead = pulse;
		   servo2.emaFilteredPulse = pulse;
		   TIMER3->TIMx_CCR3 = pulse;
	   }
	   else
	   {
			servo2.previousPulseRead = 1440;
			servo2.emaFilteredPulse = 1440;
	   }


      vTaskStartScheduler();


     while(1);
}

