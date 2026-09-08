
#pragma once
 
#include <string>
#include <vector>
 
#include "ProcessModel.h"
#include "WorkloadMetrics.h"
#include "RecommendationResult.h"
#include "BenchmarkResult.h"
#include "ExecutionResult.h"
 
class SimulationManager
{
public:
    void run(
        const std::string& csvFilePath
    );
 
private:
    void displayWorkloadMetrics(
        const WorkloadMetrics& metrics
    ) const;
 
    void displayRecommendation(
        const RecommendationResult& recommendation
    ) const;
    void runRecommendedAlgorithm(
        const std::vector<ProcessModel>& processes,
        const RecommendationResult& recommendation
    ) const;
 
 
    void displayBenchmarkResults(
        const std::vector<BenchmarkResult>& results
    ) const;
 
    void displayRanking(
        const std::vector<BenchmarkResult>& rankedResults
    ) const;
    
    void displayRecommendationAnalysis(
        const RecommendationResult& recommendation,
        const std::vector<BenchmarkResult>& rankedResults
    ) const;

    void displayExecutionTimeline(
        const ExecutionResult& result
    ) const;

    void displaySRTFVerification(
        const std::vector<ProcessModel>& processes
    ) const;
 

    const char* algorithmToString(
        AlgorithmType algorithm
    ) const;
};
