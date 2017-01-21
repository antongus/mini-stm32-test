/**
*  @file main.cpp
*
*  Main project file.
*
*  Copyright © 2016 Anton B. Gusev aka AHTOXA
**/

#include "processes.h"
#include "hw.h"
#include "adc12.h"

Process1 process1;
StatusLedProcess statusLedProcess;

OS::TEventFlag startEvent;

int main()
{
	BoardPins::Init();
	OS::run();
}

namespace OS
{
	template <>
	OS_PROCESS void Process1::exec()
	{
		OS::sleep(1);  // allow all other processes enter to wait

		adc.Init();
		startEvent.signal();

		for (;;)
		{
//			BoardPins::LedPin::On();
			for (auto i = 0; i < 500; ++i)
			{
				stepper.StepUp();
				OS::sleep(2);
			}
			OS::sleep(1000);

//			BoardPins::LedPin::Off();
			for (auto i = 0; i < 500; ++i)
			{
				stepper.StepDown();
				OS::sleep(2);
			}
			OS::sleep(1000);
		}
	}

	template <>
	OS_PROCESS void StatusLedProcess::exec()
	{
		startEvent.wait();
		for (;;)
		{
			BoardPins::LedPin::On();
			OS::sleep(100);
			BoardPins::LedPin::Off();
			OS::sleep(900);
		}
	}
}

#if scmRTOS_IDLE_HOOK_ENABLE
void OS::idle_process_user_hook()
{
	__WFI();
}
#endif
