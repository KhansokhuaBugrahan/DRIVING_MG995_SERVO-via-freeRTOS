/*
 * dma.c
 *
 *  Created on: Jan 18, 2026
 *      Author: khans
 */

#include "dma.h"


volatile uint16_t dmaBuffer[2];

void dma2Adc1Start(void)
{
	DMA2_CLOCK_ENABLE();
	DMA2_STREAM_DISABLE();
	DMA2_CIRCULAR_MODE_ENABLE();

	DMA2_DATA_LENGTH_2();
	DMA2_PSIZE_16();
	DMA2_MSIZE_16();
	DMA2_CH0_SELECT();
	DMA2_PERIP_TO_MEM_ENABLE();
	ADC1_DMA2_MATCH_DR_REG();
	DMA2_ADDRESS_INCREMENT_ENABLE();
	ADC1_DMA2_ENABLE();
	ADC1_DMA2_MATCH_BUF();
	DMA2_STREAM_ENABLE();
}
