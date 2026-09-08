
#pragma once
 
#include <string>
#include <vector>
 
#include "ProcessModel.h"
#include "ExecutionResult.h"
#include "RecommendationResult.h"
 
class SummaryReportGenerator
{
public:
    void generate(
        const std::string& applicationName,
        const std::vector<ProcessModel>& processes,
        const ExecutionResult& executionResult,
        const RecommendationResult& recommendation
    ) const;
 
private:
    std::string algorithmTypeToString(AlgorithmType algorithm) const;
 
    std::string getExecutionDate() const;
 
    std::string getSchedulingMode(
        AlgorithmType algorithm
    ) const;
 
    std::string getPerformanceCharacteristics(
        const ExecutionResult& result
    ) const;
 
    std::string getOverallResult(
        const ExecutionResult& result
    ) const;
};
