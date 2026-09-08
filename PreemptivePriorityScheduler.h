#pragma once

#include "Scheduler.h"

class PreemptivePriorityScheduler : public Scheduler
{
private:
    static const int AGING_THRESHOLD = 5;

public:
    ExecutionResult schedule(
        const std::vector<ProcessModel>& processes
    ) override;

    AlgorithmType getAlgorithmType() const override;
};
