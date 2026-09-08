
#pragma once

#include "Scheduler.h"

class PriorityScheduler : public Scheduler
{
public:
    ExecutionResult schedule(
        const std::vector<ProcessModel>& processes
    ) override;

    AlgorithmType getAlgorithmType() const override;
};
