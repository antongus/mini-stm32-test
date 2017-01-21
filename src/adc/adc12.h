/**
*  @file adc12.h
*
*  ADC12 module class
*
*  Copyright (c) 2016 by Anton Gusev aka AHTOXA
**/

#ifndef ADC12_H_INCLUDED
#define ADC12_H_INCLUDED

#include "filters.h"
#include "stm32_dma.h"
#include "scmRTOS.h"

extern "C" void DMA1_Channel1_IRQHandler();

class Adc
{
public:
	Adc() {}
	static constexpr size_t ADC_CHANNELS { 2 };
	void Init();
	OS_PROCESS void Loop();
	uint16_t Get(size_t chan);
	uint16_t operator[](size_t chan) { return Get(chan); };
	using DmaChannel = STM32::DMA::Dma1Channel1;
private:
	void SetupPins();
	void SetupTimes();
	void SetupSequence();
	void SetupTimer();
	void SetupDma();
	void SetupAdc();

	friend void DMA1_Channel1_IRQHandler();
	OS::TEventFlag event_;
	static const int channelCount { 2 };
	uint16_t results_[channelCount];

};

extern Adc adc;

#endif // ADC12_H_INCLUDED
