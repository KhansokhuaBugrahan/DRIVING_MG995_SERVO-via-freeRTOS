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
#include "adc.h"


#define    enableServo        1
#define    disableServo       0
#define    SLEW_RATE          10

 SemaphoreHandle_t semEnableServo; // created a semaphore


void vMyTask1BlinkLeds(void* pvTask1);
void vMyTask2EnableServo(void* pvTask3);
void vMyTask3GeneratePulse(void* pvTask2);

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
	static uint8_t switchServo = enableServo;
	while(1)
	{
		xSemaphoreTake(semEnableServo,portMAX_DELAY);
		if(switchServo)
		{
			TIMER3_PWM_ENABLE();
			switchServo = disableServo;
		}
		else
		{
			TIMER3_PWM_DISABLE();
			switchServo = enableServo;
		}

	}
}
void vMyTask3GeneratePulse(void* pvTask3 )
{

			static int16_t previousPulseRead = 700;
			static int16_t emaFilteredPulse;
	while(1)
	{

		if(TIMER3_ENABLED())
		{	
		     uint16_t currentAdcVal = adc1ReadValue();

			 int16_t pulseWidth = 450 + ((int32_t)currentAdcVal * 2100) / 3100;

			 emaFilteredPulse = previousPulseRead + ((pulseWidth - previousPulseRead) >> 3);

			 int16_t delta = emaFilteredPulse - previousPulseRead;

			if(abs(delta) > 5)
			{
				 if(previousPulseRead <= 2500 && previousPulseRead >= 500)
				 {
					if(delta > SLEW_RATE)
					{
						 previousPulseRead += SLEW_RATE;
						 TIMER3->TIMx_CCR1 = previousPulseRead;
					}
					else if(delta < -SLEW_RATE)
					{
						 previousPulseRead -= SLEW_RATE;
						 TIMER3->TIMx_CCR1 = previousPulseRead;
					}
				 }
			}
		}

			   vTaskDelay(pdMS_TO_TICKS(20));
	}

}

void EXTI9_5_IRQHandler(void)
{
	if(EXTI->PR & (1u << 5))
	{
		EXTI_SET_PR_5(); // clear pending register

		BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(semEnableServo, &pxHigherPriorityTaskWoken );
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
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


	              GPIO_Handle_t ADC_DIMMER_POT_PA0 = {  .PORTNAME = GPIOA,
														.PINCONF.PIN = GPIO_PIN_0,
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
				   GPIO_Handle_t EXTI5_PB5_TRIGGER = {  .PORTNAME = GPIOB,  // use as trigger input
														.PINCONF.PIN = GPIO_PIN_5,
														.PINCONF.MODE = GPIO_MODE_INT_RISING,
														.PINCONF.OTYPE = GPIO_OTYPE_PP,
														.PINCONF.OSPEED = GPIO_OSPEED_HIGH,
														.PINCONF.PUPD = GPIO_PUPD_NO,
														.PINCONF.AF = GPIO_AF_NO
													  };
				    gpioInit(&PWM_TIMER3_CH1_PB4);
					gpioInit(&ADC_DIMMER_POT_PA0);


			  	   nvicSetInterruptPriority(IRQN_EXTI9_5, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
				   nvicInterruptEnable(IRQN_EXTI9_5);
				   gpioInit(&EXTI5_PB5_TRIGGER);


				   timerxConfig(TIMER3, 16, 20000); // 1MHz --> 1us tick = 20ms 50Hz
				   timerxPeripheralEnable(TIMER3);

				   timer3PwmEnable(700);


					adc1Configuration(ADC_RESOLUTION_12, ADC_MODE_CONTINUOUS_CONV, ADC_CHANNEL_0, ADC_SAMPLING_144_CYCLE);
					adc1ChannelSequence(ADC_CHANNEL_0, ADC_CHANNEL_SEQUENCE_1);
					adc1SequenceLength(ADC_CONVERSION_LENGTH_1);
					adc1Init();



	 TaskHandle_t myTask1Handler = NULL;  //only need for if we want to manage the task after initializing

	 semEnableServo = xSemaphoreCreateBinary(); // handle semaphore whether as binary( active or  inactive)
	 xSemaphoreTake(semEnableServo, 0);

     xTaskCreate(vMyTask1BlinkLeds, "DEBUGLED1", 128, &leds[GREEN], 1, &myTask1Handler);
     xTaskCreate(vMyTask1BlinkLeds, "DEBUGLED2", 128, &leds[ORANGE], 1, &myTask1Handler);
     xTaskCreate(vMyTask1BlinkLeds, "DEBUGLED3", 128, &leds[RED], 1, &myTask1Handler);
     xTaskCreate(vMyTask1BlinkLeds, "DEBUGLED4", 128, &leds[BLUE], 1, &myTask1Handler);

     xTaskCreate(vMyTask3GeneratePulse, "DEBUGADC", 128, NULL, 2, &myTask1Handler);
     xTaskCreate(vMyTask2EnableServo, "DEBUGPWM", 128, NULL, 3, &myTask1Handler);


      vTaskStartScheduler();


     while(1);

}
