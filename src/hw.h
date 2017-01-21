/**
*  @file hw.h
*
*  Project-wide hardware declarations.
*
*  Copyright © 2016 Anton B. Gusev aka AHTOXA
**/

#ifndef HW_H_INCLUDED
#define HW_H_INCLUDED

#include "stm32.h"
#include "pin.h"
#include "scmRTOS.h"

/**
 * Board pins
 */
struct BoardPins
{
	using LedPin       = Pin<'C', 13, 'L'>; // Red LED (activated by low level on pin)

	static void Init()
	{
		LedPin::Off();
		LedPin::Mode(OUTPUT);
	}
};

/**
 * UARTs
 */
#include "stm32_uart.h"

/// Console (debug) UART properties
struct ConsoleUartProps
{
	// USART number
	static const STM32::UART::UartNum uartNum = STM32::UART::UART_1;
	// is USART remapped?
	static const STM32::UART::Remap remap = STM32::UART::REMAP_NONE;
	// USART settings
	enum
	{
		BAUDRATE = 115200,               // baud rate
		RX_BUF_SIZE = 32,                // receive buffer size
		TX_BUF_SIZE = 64,                // transmit buffer size
		UART_INTERRUPT_PRIOGROUP = 2,    // priority group for USART interrupt
		UART_INTERRUPT_SUBPRIO = 2,      // sub-priority for USART interrupt
	};
	using PinDE = STM32::UART::DummyDE ;
};

/// Console (debug) UART type
using ConsoleUart = STM32::UART::Uart<ConsoleUartProps>;

/// Console (debug) UART variable declaration
extern ConsoleUart uart;

#include "stepper.h"

struct StepperProps
{
	using Pin1 = Pin<'B', 3>;
	using Pin2 = Pin<'B', 4>;
	using Pin3 = Pin<'B', 5>;
	using Pin4 = Pin<'B', 6>;
};

using StepperType = Stepper<StepperProps>;
extern StepperType stepper;


#endif // HW_H_INCLUDED
