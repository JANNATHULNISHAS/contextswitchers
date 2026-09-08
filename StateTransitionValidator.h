#pragma once

#include "ProcessState.h"

class StateTransitionValidator
{
public:

    static bool isValid(
        ProcessState from,
        ProcessState to);
};
