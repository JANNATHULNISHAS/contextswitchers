#include "SimulationEvent.h"
 
SimulationEvent::SimulationEvent(
    const std::string& processName,
    int timestamp,
    ProcessState from,
    ProcessState to
)
    : processName(processName),
      timestamp(timestamp),
      transition(from, to)
{
}
 
const std::string&
SimulationEvent::getProcessName() const
{
    return processName;
}
 
int SimulationEvent::getTimestamp() const
{
    return timestamp;
}
 
const StateTransition&
SimulationEvent::getTransition() const
{
    return transition;
}
