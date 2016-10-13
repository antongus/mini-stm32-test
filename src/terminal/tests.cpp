/**
*  @file tests.cpp
*
*  various tests
*
*  Copyright © 2016 Anton B. Gusev aka AHTOXA
**/

#include "hw.h"
#include "processes.h"
#include "commands.h"
#include "vars.h"
#include "util.h"

INTERPRETER_COMMAND(TEST2)
{
	stream << "Test2";
	return true;
}

INTERPRETER_COMMAND(RESET)
{
	NVIC_SystemReset();
	return true;
}
