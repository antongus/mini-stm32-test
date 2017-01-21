/**
*  @file adc12.cpp
*
*  ADC12 module class implementation
*
*  Copyright (c) 2016 by Anton Gusev aka AHTOXA
**/

#include "adc12.h"
#include "pin.h"

Adc adc;

void Adc::Init()
{
	// Enable ADC peripheral
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	SetupTimer();
	SetupDma();
	SetupAdc();

	// enable DMA channel.
	DmaChannel::Enable();

	// Run timer
	TIM2->CR1 |= TIM_CR1_CEN;
}

uint16_t Adc::Get(size_t chan)
{
	return chan < ADC_CHANNELS ? results_[chan] : 0;
}


/**
 * Set up ADC pins
 */
void Adc::SetupPins()
{
	typedef Pin<'A', 0> AdcChannel0;
	typedef Pin<'A', 1> AdcChannel1;

	AdcChannel0::Direct(ANALOGINPUT);
	AdcChannel1::Direct(ANALOGINPUT);
}

/**
 * conversion times
 */
void Adc::SetupTimes()
{
	ADC1->SMPR2 = 0
			| (0x02 << 0*3) 		// channel 0 - 13.5 cycles
			| (0x02 << 1*3) 		// channel 1 - 13.5 cycles
			| (0x02 << 2*3) 		// channel 2 - 13.5 cycles
			| (0x02 << 3*3) 		// channel 3 - 13.5 cycles
			| (0x02 << 4*3) 		// channel 4
			| (0x02 << 5*3) 		// channel 5
			| (0x02 << 6*3) 		// channel 6
			| (0x02 << 7*3) 		// channel 7
			| (0x02 << 8*3)			// channel 8
			| (0x02 << 9*3)			// channel 9
			;

	ADC1->SMPR1 = 0
			| (0x02 << (10-10)*3)	// channel 10
			| (0x02 << (11-10)*3)	// channel 11
			| (0x02 << (12-10)*3)	// channel 12
			| (0x02 << (13-10)*3)	// channel 13
			| (0x02 << (14-10)*3)	// channel 14
			| (0x02 << (15-10)*3)	// channel 15
			| (0x0F << (16-10)*3)	// channel 16 - 239.5 cycles (T sensor)
			| (0x00 << (17-10)*3)	// channel 17
			;
}           

/**
 * channels sequence:
 */
void Adc::SetupSequence()
{
	ADC1->SQR3 = 0
			| 0  << (0 * 5)		// pos 1  -- ch 0
			| 1  << (1 * 5) 	// pos 2  -- ch 1
			| 2  << (2 * 5) 	// pos 3  -- ch 2
			| 3  << (3 * 5) 	// pos 4  -- ch 3
			| 4  << (4 * 5) 	// pos 5  -- ch 4
			| 5  << (5 * 5) 	// pos 6  -- ch 5
			;
			
	ADC1->SQR2 = 0
			| 6  << (0 * 5)		// pos 7  -- ch 6
			| 7  << (1 * 5) 	// pos 8  -- ch 7
			| 8  << (2 * 5) 	// pos 9  -- ch 8
			| 9  << (3 * 5) 	// pos 10 -- ch 9
			| 10 << (4 * 5) 	// pos 11 -- ch 10
			| 11 << (5 * 5) 	// pos 12 -- ch 11
			; 

	ADC1->SQR1 = 0
			| 12 << (0 * 5)		// pos 13 -- ch 12
			| 13 << (1 * 5) 	// pos 14 -- ch 13
			| 14 << (2 * 5) 	// pos 15 -- ch 14
			| 15 << (3 * 5) 	// pos 16 -- ch 15
			| (ADC_CHANNELS-1) << 20;
			; 
}            

enum
{
	ADC_TIMER_FREQ			= 100,
	ADC_TIMER_PRESCALER		= 100,
	ADC_TIMER_RELOAD 		= (36000000 / ADC_TIMER_PRESCALER / ADC_TIMER_FREQ),
	ADC_TIMER_CCR2_VAL		= (ADC_TIMER_RELOAD / 2)
};

void Adc::SetupTimer()
{
	// Enable timer peripheral
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// Set up timer
	TIM2->CR1 = TIM_CR1_ARPE;			// count up, no divisor, auto reload
	TIM2->ARR = ADC_TIMER_RELOAD;		// timer period
	TIM2->PSC = ADC_TIMER_PRESCALER;	// prescaler
	TIM2->EGR = TIM_EGR_UG;				// generate an update event to reload the prescaler value immediately

	TIM2->CR2 = 0;

	// Set up ccr2
	TIM2->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2P);	// disable cc2 output, clear polarity
	TIM2->CCR2 = ADC_TIMER_CCR2_VAL;				// cc2 period
	TIM2->CCMR1 = TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE; // PWM 1 (0x6000), enable preload;
	TIM2->CCER |= TIM_CCER_CC2E;	// enable cc2 output
}

void Adc::SetupAdc()
{
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

	SetupPins();

	ADC1->CR1 = 0
			| ADC_CR1_SCAN				// scan mode
			| ADC_CR1_JAUTO				// auto injection
			;

	ADC1->CR2 = 0
		//	| ADC_CR2_CONT				// Continuous Conversion (not needed, conversion will be triggered by timer event)
			| ADC_CR2_TSVREFE			// Temperature Sensor and VREFINT Enable
			| ADC_CR2_DMA				// DMA mode
			| ADC_CR2_EXTTRIG			// External triggering (for regular group)
			| ADC_CR2_EXTSEL_1
			| ADC_CR2_EXTSEL_0			// Select trigger event (for regular group) (011 = TIM2_CC2 event)
			;

	SetupTimes();

	SetupSequence();

	// enable ADC1
	ADC1->CR2 |= ADC_CR2_ADON;

	// reset calibration
	ADC1->CR2 |= ADC_CR2_RSTCAL;
	while (ADC1->CR2 & ADC_CR2_RSTCAL) ;

	// perform calibration
	ADC1->CR2 |= ADC_CR2_CAL;
	while (ADC1->CR2 & ADC_CR2_CAL) ;
}

void Adc::SetupDma()
{
	DmaChannel::EnableClocks();

	DmaChannel::PAR = reinterpret_cast<uint32_t>(&ADC1->DR);        // peripheral address
	DmaChannel::MAR = reinterpret_cast<uint32_t>(&results_[0]);     // memory address
	DmaChannel::NDTR = channelCount;

	DmaChannel::CR = 0
			| STM32::DMA::DMA_CR_CIRC              // Circular mode
			| STM32::DMA::DMA_CR_DIR_PERITH_TO_MEM // direction
			| STM32::DMA::DMA_CR_MINC              // memory increment mode
			| STM32::DMA::DMA_CR_PSIZE_16_BIT      // peripheral size = halfword
			| STM32::DMA::DMA_CR_MSIZE_16_BIT      // memory size = halfword
			| STM32::DMA::DMA_CR_PRIO_MED          // channel priority level
			| STM32::DMA::DMA_CR_TCIE              // transfer-complete interrupt enable
			;

	NVIC_SetPriority(DmaChannel::DMAChannel_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 1));
	NVIC_EnableIRQ(DmaChannel::DMAChannel_IRQn);
}

OS_PROCESS void Adc::Loop()
{
	for (;;)
	{
		event_.wait();
	}
}

extern "C" void DMA1_Channel1_IRQHandler(void)
{
	if (Adc::DmaChannel::ISR & Adc::DmaChannel::DMA_MASK_TCIF)
	{
		OS::TISRW ISR;
		Adc::DmaChannel::IFCR = Adc::DmaChannel::DMA_MASK_TCIF;
		adc.event_.signal_isr();
	}
}
