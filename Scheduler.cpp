#include "Scheduler.h"
 
#include "SimulationEvent.h"
 
Scheduler::Scheduler()
{
}
 
Scheduler::~Scheduler()
{
}
 
void Scheduler::setObserver(ISimulationObserver* newObserver)
{
    observers.clear();
 
    if (newObserver != 0)
    {
        observers.push_back(newObserver);
    }
}
 
void Scheduler::addObserver(ISimulationObserver* newObserver)
{
    if (newObserver != 0)
    {
        observers.push_back(newObserver);
    }
}

 
void Scheduler::notifyEvent(const SimulationEvent& event) const
{
    for (std::size_t i = 0; i < observers.size(); ++i)
    {
        if (observers[i] != 0)
        {
            observers[i]->onEvent(event);
        }
    }
}
 
void Scheduler::changeProcessState(
    ProcessModel& process,
    ProcessState newState,
    int timestamp) const
{
    const ProcessState oldState =
        process.getState();
 
    process.changeState(newState);
 
    const ProcessState currentState =
        process.getState();
 
    if (oldState != currentState)
    {
        notifyEvent(
            SimulationEvent(
                process.getName(),
                timestamp,
                oldState,
                currentState
            )
        );
    }
}
