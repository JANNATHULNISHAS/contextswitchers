#pragma once
 
#include <vector>
 
#include "ProcessModel.h"
#include "ExecutionResult.h"
#include "AlgorithmType.h"
#include "ISimulationObserver.h"
 
class Scheduler
{
private:
    std::vector<ISimulationObserver*> observers;
 
public:
    Scheduler();
    virtual ~Scheduler();
 
    void setObserver(ISimulationObserver* observer);
    void addObserver(ISimulationObserver* observer);
 
protected:
    void notifyEvent(const SimulationEvent& event) const;
 
    void changeProcessState(
        ProcessModel& process,
        ProcessState newState,
        int timestamp
    ) const;
 
public:
    virtual ExecutionResult schedule(
        const std::vector<ProcessModel>& processes
    ) = 0;
 
    virtual AlgorithmType getAlgorithmType() const = 0;
};
