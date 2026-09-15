#include "SimulationEvent.h"

SimulationEvent::SimulationEvent(
    const std::string& processName,
    int timestamp,
    SimulationEventType eventType,
    const std::string& details
)
    : processName(processName),
      timestamp(timestamp),
      eventType(eventType),
      transition(
          ProcessState::NEW,
          ProcessState::NEW
      ),
      details(details)
{
}

SimulationEvent::SimulationEvent(
    const std::string& processName,
    int timestamp,
    ProcessState from,
    ProcessState to
)
    : processName(processName),
      timestamp(timestamp),
      eventType(
          SimulationEventType::STATE_CHANGE
      ),
      transition(from, to),
      details("")
{
}

const std::string&
SimulationEvent::getProcessName() const
{
    return processName;
}

int
SimulationEvent::getTimestamp() const
{
    return timestamp;
}

SimulationEventType
SimulationEvent::getEventType() const
{
    return eventType;
}

const StateTransition&
SimulationEvent::getTransition() const
{
    return transition;
}

const std::string&
SimulationEvent::getDetails() const
{
    return details;
}
