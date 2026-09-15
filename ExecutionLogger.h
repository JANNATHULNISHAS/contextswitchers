#pragma once
 
#include <fstream>
#include <string>
#include <vector>
 
#include "ISimulationObserver.h"
#include "SimulationEvent.h"
#include "SimulationManager.h"
 
class ExecutionLogger : public ISimulationObserver
{
private:
    std::ofstream logFile;
 
    std::vector<SimulationEvent> stateTransitions;
 
public:
    ExecutionLogger();
 
    ~ExecutionLogger();
 
    bool start(
        const std::string& filePath
    );
 
    void stop();
 
    virtual void onEvent(
        const SimulationEvent& event
    );
 
    const std::vector<SimulationEvent>&
    getStateTransitions() const;
 
private:
    const char* eventTypeToString(
        SimulationEventType eventType
    ) const;
 
    const char* processStateToString(
        ProcessState state
    ) const;
};
 
