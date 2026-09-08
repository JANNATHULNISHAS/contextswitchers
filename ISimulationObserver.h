
#pragma once
 
class SimulationEvent;
 
class ISimulationObserver
{
public:
    virtual ~ISimulationObserver()
    {
    }
 
    virtual void onEvent(
        const SimulationEvent& event
    ) = 0;
};
