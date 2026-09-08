
#pragma once
 
#include <string>
#include <vector>
 
#include "ProcessModel.h"
#include "WorkloadMetrics.h"
#include "RecommendationResult.h"
#include "AlgorithmType.h"
 
class WorkloadReportGenerator
{
public:
    void generate(
        const std::string& applicationName,
        const std::string& workloadSource,
        const std::vector<ProcessModel>& processes,
        const WorkloadMetrics& workloadMetrics,
        const RecommendationResult& recommendation
    ) const;
 
private:
    std::string algorithmTypeToString(AlgorithmType algorithm) const;
 
    std::string arrivalPatternToString(
        ArrivalPattern pattern
    ) const;
 
    std::string varianceLevelToString(
        VarianceLevel level
    ) const;
 
    std::string workloadTypeToString(
        WorkloadType type
    ) const;
};
