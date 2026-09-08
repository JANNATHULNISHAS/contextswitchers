
#pragma once
 
#include "Scheduler.h"
 
class RoundRobinScheduler : public Scheduler
{
private:
    int timeQuantum;
 
public:
    explicit RoundRobinScheduler(int quantum = 4);
 
    ExecutionResult schedule(
        const std::vector<ProcessModel>& processes
    ) override;
 
    AlgorithmType getAlgorithmType() const override;
 
    int getTimeQuantum() const;
};
