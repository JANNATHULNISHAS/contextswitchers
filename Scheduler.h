
#pragma once

#include <vector>
#include "ProcessModel.h"
#include "ExecutionResult.h"
#include "AlgorithmType.h"

class Scheduler
{
public:
    virtual ~Scheduler();

    virtual ExecutionResult schedule(
        const std::vector<ProcessModel>& processes
    ) = 0;

    virtual AlgorithmType getAlgorithmType() const = 0;
};
