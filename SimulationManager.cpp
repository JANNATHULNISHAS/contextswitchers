#include "SimulationManager.h"
#include "SchedulerFactory.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "RankingEngine.h"
#include "WorkloadLoader.h"
#include "WorkloadAnalyzer.h"
#include "RecommendationEngine.h"
#include "BenchmarkEngine.h"
#include "MetricsCalculator.h"
#include "SRTFScheduler.h"


void SimulationManager::run(
    const std::string& csvFilePath)
{
    try
    {
        // -----------------------------------------
        // 1. Load workload
        // -----------------------------------------

        WorkloadLoader loader;

        std::vector<ProcessModel> processes =
            loader.loadCSV(csvFilePath);

        if (processes.empty())
        {
            std::cout
                << "No processes found in workload."
                << std::endl;

            return;
        }

        std::cout << std::endl;

        std::cout
            << "========================================"
            << std::endl;

        std::cout
            << "        CPU SCHEDULER ANALYSIS"
            << std::endl;

        std::cout
            << "========================================"
            << std::endl;

        std::cout
            << "Workload: "
            << csvFilePath
            << std::endl;

        std::cout
            << "Processes loaded: "
            << processes.size()
            << std::endl;


        // -----------------------------------------
        // 2. Analyze workload
        // -----------------------------------------

        WorkloadAnalyzer analyzer;

        WorkloadMetrics metrics =
            analyzer.analyze(processes);

        displayWorkloadMetrics(metrics);


        // -----------------------------------------
        // 3. Recommend algorithm
        // -----------------------------------------

        RecommendationEngine recommendationEngine;

        RecommendationResult recommendation =
            recommendationEngine.recommend(metrics);

        displayRecommendation(recommendation);

        runRecommendedAlgorithm(
            processes,
            recommendation
        );


        // -----------------------------------------
        // 4. SRTF verification
        // -----------------------------------------

        displaySRTFVerification(processes);


        // -----------------------------------------
        // 5. Benchmark algorithms
        // -----------------------------------------

        BenchmarkEngine benchmarkEngine;

        std::vector<BenchmarkResult> benchmarkResults =
            benchmarkEngine.benchmark(processes);

        displayBenchmarkResults(
            benchmarkResults
        );


        // -----------------------------------------
        // 6. Rank algorithms
        // -----------------------------------------

        RankingEngine rankingEngine;

        std::vector<BenchmarkResult> rankedResults =
            rankingEngine.rank(
                benchmarkResults
            );

        displayRanking(
            rankedResults
        );


        // -----------------------------------------
        // 7. Recommendation vs benchmark
        // -----------------------------------------

        displayRecommendationAnalysis(
            recommendation,
            rankedResults
        );


        std::cout
            << std::endl
            << "Analysis completed."
            << std::endl;
    }
    catch (const std::exception& error)
    {
        std::cout
            << "Simulation error: "
            << error.what()
            << std::endl;
    }
}


void SimulationManager::displayWorkloadMetrics(
    const WorkloadMetrics& metrics) const
{
    std::cout << std::endl;

    std::cout
        << "--------------- WORKLOAD ---------------"
        << std::endl;

    std::cout
        << "Process Count             : "
        << metrics.getProcessCount()
        << std::endl;

    std::cout
        << "Average Burst Time       : "
        << std::fixed
        << std::setprecision(2)
        << metrics.getAverageBurstTime()
        << std::endl;

    std::cout
        << "Minimum Burst Time       : "
        << metrics.getMinimumBurstTime()
        << std::endl;

    std::cout
        << "Maximum Burst Time       : "
        << metrics.getMaximumBurstTime()
        << std::endl;

    std::cout
        << "Burst Variance            : "
        << metrics.getBurstVariance()
        << std::endl;

    std::cout
        << "Priority Levels           : "
        << metrics.getPriorityLevelCount()
        << std::endl;

    std::cout
        << "Priority-1 Processes      : "
        << metrics.getHighestPriorityProcessCount()
        << std::endl;

    std::cout
        << "I/O Processes             : "
        << metrics.getIOProcessCount()
        << std::endl;

    std::cout
        << "-----------------------------------------"
        << std::endl;
}


void SimulationManager::displayRecommendation(
    const RecommendationResult& recommendation) const
{
    std::cout << std::endl;

    std::cout
        << "------------ RECOMMENDATION -------------"
        << std::endl;

    std::cout
        << "Recommended Algorithm     : "
        << algorithmToString(
               recommendation.getRecommendedAlgorithm())
        << std::endl;

    std::cout
        << "Confidence                 : "
        << std::fixed
        << std::setprecision(1)
        << recommendation.getConfidence()
        << "%"
        << std::endl;

    std::cout
        << "Reason                     : "
        << recommendation.getReason()
        << std::endl;

    std::cout
        << "-----------------------------------------"
        << std::endl;
}


void SimulationManager::runRecommendedAlgorithm(
    const std::vector<ProcessModel>& processes,
    const RecommendationResult& recommendation) const
{
    std::cout << std::endl;

    std::cout
        << "------- RECOMMENDED ALGORITHM RUN -------"
        << std::endl;

    const AlgorithmType algorithm =
        recommendation.getRecommendedAlgorithm();

    std::unique_ptr<Scheduler> scheduler =
        SchedulerFactory::create(algorithm);

    if (!scheduler)
    {
        std::cout
            << "Unable to create recommended scheduler."
            << std::endl;

        return;
    }

    ExecutionResult result =
        scheduler->schedule(processes);

    MetricsCalculator calculator;

    calculator.calculate(
        processes,
        result
    );

    std::cout
        << "Algorithm                 : "
        << algorithmToString(algorithm)
        << std::endl;

    std::cout
        << "Average Waiting Time      : "
        << std::fixed
        << std::setprecision(2)
        << result.getAverageWaitingTime()
        << std::endl;

    std::cout
        << "Average Turnaround Time   : "
        << result.getAverageTurnaroundTime()
        << std::endl;

    std::cout
        << "Average Response Time     : "
        << result.getAverageResponseTime()
        << std::endl;

    std::cout
        << "CPU Utilization           : "
        << result.getCPUUtilization()
        << "%"
        << std::endl;

    std::cout
        << "Context Switches          : "
        << result.getContextSwitches()
        << std::endl;

    std::cout
        << "Preemptions               : "
        << result.getPreemptions()
        << std::endl;

    std::cout
        << "-----------------------------------------"
        << std::endl;
}


/*
 * Display the actual CPU execution timeline.
 *
 * Gaps between execution slices are displayed
 * as CPU IDLE periods.
 */
void SimulationManager::displayExecutionTimeline(
    const ExecutionResult& result) const
{
    const std::vector<ExecutionSlice>& timeline =
        result.getTimeline();

    if (timeline.empty())
    {
        std::cout
            << "No execution timeline available."
            << std::endl;

        return;
    }

    std::cout << std::endl;

    std::cout
        << "------------- EXECUTION TIMELINE --------"
        << std::endl;

    std::cout
        << std::left
        << std::setw(12) << "Start"
        << std::setw(12) << "End"
        << std::setw(25) << "Process"
        << std::endl;

    std::cout
        << "--------------------------------------------------"
        << std::endl;

    int previousEndTime = 0;

    for (std::size_t i = 0;
         i < timeline.size();
         ++i)
    {
        const ExecutionSlice& slice =
            timeline[i];

        // Detect CPU idle time between execution slices.
        if (slice.getStartTime() > previousEndTime)
        {
            std::cout
                << std::left
                << std::setw(12)
                << previousEndTime
                << std::setw(12)
                << slice.getStartTime()
                << std::setw(25)
                << "IDLE"
                << std::endl;
        }

        std::cout
            << std::left
            << std::setw(12)
            << slice.getStartTime()
            << std::setw(12)
            << slice.getEndTime()
            << std::setw(25)
            << slice.getProcessName()
            << std::endl;

        previousEndTime =
            slice.getEndTime();
    }

    std::cout
        << "--------------------------------------------------"
        << std::endl;

    std::cout
        << "Total Execution Time : "
        << result.getTotalExecutionTime()
        << std::endl;

    std::cout
        << "CPU Busy Time        : "
        << result.getCPUBusyTime()
        << std::endl;

    std::cout
        << "CPU Idle Time        : "
        << result.getCPUIdleTime()
        << std::endl;

    std::cout
        << "CPU Utilization      : "
        << std::fixed
        << std::setprecision(2)
        << result.getCPUUtilization()
        << "%"
        << std::endl;

    std::cout
        << "-----------------------------------------"
        << std::endl;
}


/*
 * Run SRTF separately so we can verify its
 * CPU busy/idle behavior using the actual timeline.
 *
 * This is NOT part of the benchmark engine.
 */
void SimulationManager::displaySRTFVerification(
    const std::vector<ProcessModel>& processes) const
{
    SRTFScheduler scheduler;

    ExecutionResult result =
        scheduler.schedule(processes);

    MetricsCalculator calculator;

    calculator.calculate(
        processes,
        result
    );

    std::cout << std::endl;

    std::cout
        << "----------- SRTF VERIFICATION -----------"
        << std::endl;

    displayExecutionTimeline(result);
}


/*
 * Display benchmark results.
 */
void SimulationManager::displayBenchmarkResults(
    const std::vector<BenchmarkResult>& results) const
{
    std::cout << std::endl;

    std::cout
        << "------------- BENCHMARK -----------------"
        << std::endl;

    if (results.empty())
    {
        std::cout
            << "No benchmark results."
            << std::endl;

        return;
    }

    std::cout
        << std::left
        << std::setw(27) << "Algorithm"
        << std::right
        << std::setw(12) << "Avg WT"
        << std::setw(12) << "Avg TAT"
        << std::setw(12) << "Avg RT"
        << std::setw(12) << "CPU %"
        << std::setw(12) << "Avg CST"
        << std::setw(10) << "CS"
        << std::setw(10) << "Preempt"
        << std::endl;

    std::cout
        << "--------------------------------------------------------------------------"
        << std::endl;

    for (std::size_t i = 0;
         i < results.size();
         ++i)
    {
        const BenchmarkResult& result =
            results[i];

        std::cout
            << std::left
            << std::setw(27)
            << algorithmToString(
                   result.getAlgorithm())
            << std::right
            << std::fixed
            << std::setprecision(2)
            << std::setw(12)
            << result.getAverageWaitingTime()
            << std::setw(12)
            << result.getAverageTurnaroundTime()
            << std::setw(12)
            << result.getAverageResponseTime()
            << std::setw(12)
            << result.getCPUUtilization()
            << std::setw(12)
            << result.getAverageContextSwitchTime()
            << std::setw(10)
            << result.getContextSwitches()
            << std::setw(10)
            << result.getPreemptions()
            << std::endl;
    }

    std::cout
        << "--------------------------------------------------------------------------"
        << std::endl;
}


/*
 * Display ranking.
 */
void SimulationManager::displayRanking(
    const std::vector<BenchmarkResult>& rankedResults) const
{
    std::cout << std::endl;

    std::cout
        << "--------------- RANKING ----------------"
        << std::endl;

    std::cout
        << std::left
        << std::setw(8) << "Rank"
        << std::setw(30) << "Algorithm"
        << std::right
        << std::setw(12) << "Score"
        << std::endl;

    std::cout
        << "--------------------------------------------------"
        << std::endl;

    for (std::size_t i = 0;
         i < rankedResults.size();
         ++i)
    {
        const BenchmarkResult& result =
            rankedResults[i];

        std::cout
            << std::left
            << std::setw(8)
            << result.getRank()
            << std::setw(30)
            << algorithmToString(
                   result.getAlgorithm())
            << std::right
            << std::setw(12)
            << std::fixed
            << std::setprecision(2)
            << result.getRankingScore()
            << std::endl;
    }

    std::cout
        << "--------------------------------------------------"
        << std::endl;
}


/*
 * Compare workload recommendation against
 * benchmark winner.
 */
void SimulationManager::displayRecommendationAnalysis(
    const RecommendationResult& recommendation,
    const std::vector<BenchmarkResult>& rankedResults) const
{
    std::cout << std::endl;

    std::cout
        << "------ RECOMMENDATION vs BENCHMARK ------"
        << std::endl;

    if (rankedResults.empty())
    {
        std::cout
            << "No benchmark results available."
            << std::endl;

        return;
    }

    const AlgorithmType recommended =
        recommendation.getRecommendedAlgorithm();

    const AlgorithmType benchmarkWinner =
        rankedResults[0].getAlgorithm();

    std::cout
        << "Recommended Algorithm : "
        << algorithmToString(recommended)
        << std::endl;

    std::cout
        << "Benchmark Winner      : "
        << algorithmToString(benchmarkWinner)
        << std::endl;

    if (recommended == benchmarkWinner)
    {
        std::cout
            << "Result                : Recommendation matches benchmark winner."
            << std::endl;
    }
    else
    {
        std::cout
            << "Result                : Recommendation differs from benchmark winner."
            << std::endl;

        std::cout
            << "Observation            : The recommendation is workload-based, "
            << "while the benchmark winner is performance-based."
            << std::endl;
    }

    std::cout
        << "-----------------------------------------"
        << std::endl;
}


/*
 * Convert algorithm enum to display text.
 */
const char* SimulationManager::algorithmToString(
    AlgorithmType algorithm) const
{
    switch (algorithm)
    {
        case AlgorithmType::FCFS:
            return "FCFS";

        case AlgorithmType::SJF:
            return "SJF";

        case AlgorithmType::SRTF:
            return "SRTF";

        case AlgorithmType::PRIORITY_NON_PREEMPTIVE:
            return "Priority Non-Preemptive";

        case AlgorithmType::PRIORITY_PREEMPTIVE:
            return "Priority Preemptive";

        case AlgorithmType::ROUND_ROBIN:
            return "Round Robin";
    }

    return "Unknown";
}
