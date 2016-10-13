/**
*  @file processes.h
*
*  Process classes declaration.
*
*  Copyright © 2016 Anton B. Gusev aka AHTOXA
**/

#ifndef PROCESSES_H_INCLUDED
#define PROCESSES_H_INCLUDED

#include "scmRTOS.h"

using Process1              = OS::process<OS::pr0, 1200>;
using StatusLedProcess      = OS::process<OS::pr1, 1200>;
using DebugTerminalProcess  = OS::process<OS::pr2, 1200>;

extern Process1 process1;
extern StatusLedProcess statusLedProcess;
extern DebugTerminalProcess debugTerminalProcess;

#endif // PROCESSES_H_INCLUDED
