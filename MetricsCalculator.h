
#pragma once
 
#include <vector>
 
#include "ProcessModel.h"
#include "ExecutionResult.h"
 
class MetricsCalculator
{
public:
    void calculate(
        const std::vector<ProcessModel>& processes,
        ExecutionResult& result
    ) const;
};
