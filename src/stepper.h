/**
*  @file stepper.h
*
*  Created on: 15 окт. 2016 г.
*  Copyright © 2016 Anton B. Gusev aka AHTOXA
**/


#ifndef STEPPER_H_
#define STEPPER_H_

#include "pin.h"

struct StepperSampleProps
{
	using Pin1 = Pin<'B', 3>;
	using Pin2 = Pin<'B', 4>;
	using Pin3 = Pin<'B', 5>;
	using Pin4 = Pin<'B', 6>;
};

template <class props>
class Stepper
{
public:
	Stepper()
	{
		Pin1::Off();
		Pin2::Off();
		Pin3::Off();
		Pin4::Off();
		Pin1::Mode(OUTPUT);
		Pin2::Mode(OUTPUT);
		Pin3::Mode(OUTPUT);
		Pin4::Mode(OUTPUT);
	}
	void StepUp()
	{
		if (++pos_ >= programLen)
			pos_ = 0;
		ApplyPos();
	}
	void StepDown()
	{
		if (pos_)
			--pos_;
		else
			pos_ =  programLen - 1;
		ApplyPos();
	}

private:
	using Pin1 = typename props::Pin1;
	using Pin2 = typename props::Pin2;
	using Pin3 = typename props::Pin3;
	using Pin4 = typename props::Pin4;

	static const size_t programLen { 8 };
	size_t pos_ { 0 };

	void ApplyPos()
	{
//		static uint8_t program[programLen] =  {0x9, 0x1, 0x7, 0x6, 0xE, 0x8};
		static uint8_t program[programLen] =  { 0x01, 0x03, 0x02, 0x06, 0x04, 0x0C, 0x08, 0x09 };
//		static uint8_t program[programLen] =  { 0x01, 0x02, 0x04, 0x08 };
		auto currentValue = program[pos_];
		Pin1::On(currentValue & 0x01);
		Pin2::On(currentValue & 0x02);
		Pin3::On(currentValue & 0x04);
		Pin4::On(currentValue & 0x08);
	}
};



#endif // STEPPER_H_
