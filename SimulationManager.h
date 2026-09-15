#pragma once
 
#include <string>
#include <vector>
 
#include "ProcessModel.h"
#include "WorkloadMetrics.h"
#include "RecommendationResult.h"
#include "BenchmarkResult.h"
#include "ExecutionResult.h"
#include "SimulationEvent.h"

class SimulationManager
{
public:
  /*  void run(
        const std::string& csvFilePath
    );*/
    void run(
    const std::string& csvFilePath,
    const std::string& applicationName
);
private:
    void displayWorkloadMetrics(
        const WorkloadMetrics& metrics
    ) const;

    void generateWorkloadAnalysisReport(
    const std::string& csvFilePath,
    const std::string& applicationName,
    const WorkloadMetrics& metrics,
    const RecommendationResult& recommendation
    ) const;

    void generateSummaryReport(
    const std::string& applicationName,
    const RecommendationResult& recommendation,
    const ExecutionResult& executionResult,
    const std::vector<BenchmarkResult>& rankedResults
    ) const;

    void generateBenchmarkReport(
    const std::string& applicationName,
    const std::vector<BenchmarkResult>& rankedResults,
    const RecommendationResult& recommendation
    ) const;
    
    void generateProcessReport(
    const std::vector<ProcessModel>& processes,
    const ExecutionResult& executionResult
    ) const;

    void generateStateTransitionReport(
    const std::string& applicationName,
    const std::vector<SimulationEvent>& transitions
) const;

    void displayRecommendation(
        const RecommendationResult& recommendation
    ) const;

    ExecutionResult runRecommendedAlgorithm(
    const std::vector<ProcessModel>& processes,
    const RecommendationResult& recommendation,
    const std::string& applicationName
    ) const;

    void displayRecommendedExecution(
    const ExecutionResult& result
) const;
 
//bool askRunOtherAlgorithms() const;
    

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
    
    void displayGanttChart(
    const ExecutionResult& result
    ) const;

    void displaySRTFVerification(
        const std::vector<ProcessModel>& processes
    ) const;
 

    const char* algorithmToString(
        AlgorithmType algorithm
    ) const;
};
