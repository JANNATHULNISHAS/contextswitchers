
#pragma once

#include "Scheduler.h"

class SJFScheduler : public Scheduler
{
public:
    ExecutionResult schedule(
        const std::vector<ProcessModel>& processes
    ) override;

    AlgorithmType getAlgorithmType() const override;
};
