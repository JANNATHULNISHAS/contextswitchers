
#pragma once

#include <vector>
#include "ProcessModel.h"
#include "WorkloadMetrics.h"

class WorkloadAnalyzer
{
public:
    WorkloadMetrics analyze(
        const std::vector<ProcessModel>& processes
    ) const;
};
