
#pragma once
 
#include <string>
 
#include "StateTransition.h"
 
class SimulationEvent
{
private:
    std::string processName;
    int timestamp;
    StateTransition transition;
 
public:
    SimulationEvent(
        const std::string& processName,
        int timestamp,
        ProcessState from,
        ProcessState to
    );
 
    const std::string& getProcessName() const;
 
    int getTimestamp() const;
 
    const StateTransition& getTransition() const;
};
