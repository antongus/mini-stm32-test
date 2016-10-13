/**
*  @file terminal.cpp
*
*  Serial port debug terminal.
*
*  Copyright © 2016 Anton B. Gusev aka AHTOXA
**/

#include "hw.h"
#include "processes.h"
#include "commands.h"
#include "vars.h"
#include "util.h"

namespace OS
{
	template <>
	OS_PROCESS void DebugTerminalProcess::exec()
	{
		char buf[81];
		//startEvent.wait();
		for (;;)
		{
			uart << "=>";
			uart.Gets(buf, 60);
			if (*buf)
			{
				if (!Interpreter::parse(buf, uart))
					uart << "ERR";
				uart << "\r\n";
			}
		}
	}
}

DebugTerminalProcess debugTerminalProcess;

INTERPRETER_COMMAND(TEST)
{
	stream << "Test!";
	return true;
}


INTERPRETER_COMMAND(GET)
{
	char *lt = args;
	char *token = GetToken(0, &lt);

	if (token)
	{
		Variable * v = VarList::Find(token);
		if (v)
			return v->Get(lt, stream);
	}
	return false;
}

INTERPRETER_COMMAND(SET)
{
	char *lt = args;
	char *token = GetToken(0, &lt);

	if (token)
	{
		Variable * v = VarList::Find(token);
		if (v)
			return v->Set(lt, stream);
	}
	return false;
}
