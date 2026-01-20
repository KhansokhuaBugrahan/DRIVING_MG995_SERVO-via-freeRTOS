/*
 * DMA.h
 *
 *  Created on: Jan 17, 2026
 *      Author: khans
 */

#ifndef DMA_H_
#define DMA_H_

#include <stdint.h>
#include "stm32f4xxx.h"
#include "adc.h"

#define 	ADC_CHANNEL_COUNT    2

extern volatile uint16_t dmaBuffer[ADC_CHANNEL_COUNT];

typedef struct {

	volatile uint32_t DMA_LISR;
	volatile uint32_t DMA_HISR;
	volatile uint32_t DMA_LIFCR;
	volatile uint32_t DMA_HIFCR;
	volatile uint32_t DMA_S0CR;
	volatile uint32_t DMA_S0NDTR;
	volatile uint32_t DMA_S0PAR;
	volatile uint32_t DMA_S0M0AR;
	volatile uint32_t DMA_S0M1AR;
	volatile uint32_t DMA_S0FCR;
	volatile uint32_t DMA_S1CR;
	volatile uint32_t DMA_S1NDTR;

}DMA_RegDef_t;

#define   	 DMA1      ((DMA_RegDef_t*)(DMA1_BASE_ADDR))
#define   	 DMA2      ((DMA_RegDef_t*)(DMA2_BASE_ADDR))

#define        DMA2_CLOCK_ENABLE()              (RCC->AHB1ENR |= (1u << 22))
#define   	   DMA2_STREAM_DISABLE()  			(DMA2->DMA_S0CR &= ~(1u << 0 ))
#define   	   DMA2_STREAM_ENABLE()  			(DMA2->DMA_S0CR |= (1u << 0 ))
#define   	   DMA2_CIRCULAR_MODE_ENABLE()  	(DMA2->DMA_S0CR |= (1u << 8 ))
#define        DMA2_DATA_LENGTH_2()             (DMA2->DMA_S0NDTR = 2)
#define        DMA2_PSIZE_16()                  (DMA2->DMA_S0CR |= (1u << 11))
#define        DMA2_MSIZE_16()                  (DMA2->DMA_S0CR |= (1u << 13))
#define        DMA2_CH0_SELECT()				(DMA2->DMA_S0CR &= ~(0b111 << 25))
#define        DMA2_PERIP_TO_MEM_ENABLE()       (DMA2->DMA_S0CR &= ~(0b11 << 6))
#define        ADC1_DMA2_MATCH_DR_REG()         (DMA2->DMA_S0PAR = (uint32_t)&ADC_1->ADC_DR)
#define        ADC1_DMA2_MATCH_BUF()            (DMA2->DMA_S0M0AR = (uint32_t)&dmaBuffer)
#define        DMA2_ADDRESS_INCREMENT_ENABLE()  (DMA2->DMA_S0CR |= (1u << 10))
#define        ADC1_DMA2_ENABLE()               (ADC_1->ADC_CR2 |= (0b11 << 8))


void dma2Adc1Start(void);

#endif /* DMA_H_ */
