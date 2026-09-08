
#pragma once

#include "ProcessState.h"

struct StateTransition
{
    ProcessState from;
    ProcessState to;

    StateTransition(ProcessState fromState, ProcessState toState)
        : from(fromState), to(toState)
    {
    }
};
