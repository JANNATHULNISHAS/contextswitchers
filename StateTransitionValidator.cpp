#include "StateTransitionValidator.h"

bool StateTransitionValidator::isValid(
    ProcessState from,
    ProcessState to
)
{
    if (from == ProcessState::NEW &&
        to == ProcessState::READY)
    {
        return true;
    }

    if (from == ProcessState::READY &&
        to == ProcessState::RUNNING)
    {
        return true;
    }

    if (from == ProcessState::RUNNING &&
        (to == ProcessState::READY ||
         to == ProcessState::WAITING ||
         to == ProcessState::COMPLETED))
    {
        return true;
    }

    if (from == ProcessState::WAITING &&
        to == ProcessState::READY)
    {
        return true;
    }

    return false;
}
