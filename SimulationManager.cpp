#include "SimulationManager.h"
#include "SchedulerFactory.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <fstream>

#include "LiveDashboard.h"
#include "ExecutionLogger.h"
#include "RankingEngine.h"
#include "WorkloadLoader.h"
#include "WorkloadAnalyzer.h"
#include "RecommendationEngine.h"
#include "BenchmarkEngine.h"
#include "MetricsCalculator.h"
#include "SRTFScheduler.h"

static const char* processStateToString(ProcessState state)
{
    switch (state)
    {
        case ProcessState::NEW:
            return "NEW";
 
        case ProcessState::READY:
            return "READY";
 
        case ProcessState::RUNNING:
            return "RUNNING";
 
        case ProcessState::WAITING:
            return "WAITING";
 
        case ProcessState::COMPLETED:
            return "COMPLETED";
    }
 
    return "UNKNOWN";
}


void SimulationManager::run(
    const std::string& csvFilePath,
    const std::string& applicationName)
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

        generateWorkloadAnalysisReport(csvFilePath,applicationName,
        metrics,
        recommendation
        );

        ExecutionResult recommendedResult =
        runRecommendedAlgorithm(
        processes,
        recommendation,
        applicationName
        );
 

 
// -----------------------------------------
// Benchmark other algorithms
// -----------------------------------------
 
BenchmarkEngine benchmarkEngine;
 
std::vector<BenchmarkResult> benchmarkResults =
    benchmarkEngine.benchmark(processes);

    displayBenchmarkResults(benchmarkResults);
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
        
        generateBenchmarkReport(
        applicationName,
        rankedResults,
        recommendation
        );

        // -----------------------------------------
        // 7. Recommendation vs benchmark
        // -----------------------------------------

        displayRecommendationAnalysis(
            recommendation,
            rankedResults
        );
        
        generateSummaryReport(
    applicationName,
    recommendation,
    recommendedResult,
    rankedResults
);

    generateProcessReport(
    processes,
    recommendedResult
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

void SimulationManager::generateWorkloadAnalysisReport(
    const std::string& csvFilePath,
    const std::string& applicationName,
    const WorkloadMetrics& metrics,
    const RecommendationResult& recommendation
) const
{
    std::ofstream report(
        "reports/workload_analysis_report.txt"
    );
 
    if (!report.is_open())
    {
        std::cout
            << "Warning: Unable to create workload analysis report."
            << std::endl;
        return;
    }
 
    report
        << "============================================================\n"
        << "              WORKLOAD ANALYSIS REPORT\n"
        << "============================================================\n\n";
 
    report
        << "Application        : "
        << applicationName << "\n";
 
    report
        << "Workload File      : "
        << csvFilePath << "\n\n";
 
    report
        << "--------------- WORKLOAD CHARACTERISTICS ----------------\n\n";
 
    report
        << "Process Count      : "
        << metrics.getProcessCount() << "\n";
 
    report
        << std::fixed
        << std::setprecision(2);
 
    report
        << "Average Burst Time : "
        << metrics.getAverageBurstTime() << "\n";
 
    report
        << "Minimum Burst Time : "
        << metrics.getMinimumBurstTime() << "\n";
 
    report
        << "Maximum Burst Time : "
        << metrics.getMaximumBurstTime() << "\n";
 
    report
        << "Burst Variance     : "
        << metrics.getBurstVariance() << "\n";
 
    report
        << "Priority Levels    : "
        << metrics.getPriorityLevelCount() << "\n";
 
    report
        << "Priority-1 Count   : "
        << metrics.getHighestPriorityProcessCount() << "\n";
 
    report
        << "I/O Processes      : "
        << metrics.getIOProcessCount() << "\n";
 
 report
    << "Arrival Pattern    : ";
 
switch (metrics.getArrivalPattern())
{
    case ArrivalPattern::BATCH:
        report << "Batch";
        break;
 
    case ArrivalPattern::INTERACTIVE:
        report << "Interactive";
        break;
 
    case ArrivalPattern::MIXED:
        report << "Mixed";
        break;
}
 
report << "\n";
 
report
    << "Variance Level     : ";
 
switch (metrics.getVarianceLevel())
{
    case VarianceLevel::LOW:
        report << "Low";
        break;
 
    case VarianceLevel::MEDIUM:
        report << "Medium";
        break;
 
    case VarianceLevel::HIGH:
        report << "High";
        break;
}
 
report << "\n";
 
report
    << "Workload Type      : ";
 
switch (metrics.getWorkloadType())
{
    case WorkloadType::BATCH:
        report << "Batch";
        break;
 
    case WorkloadType::INTERACTIVE:
        report << "Interactive";
        break;
 
    case WorkloadType::PRIORITY_CRITICAL:
        report << "Priority-Critical";
        break;
 
    case WorkloadType::MIXED:
        report << "Mixed";
        break;
}
 
    report << "\n\n";

    report
        << "--------------- RECOMMENDATION ----------------------------\n\n";
 
    report
        << "Recommended Algorithm : "
        << algorithmToString(
               recommendation.getRecommendedAlgorithm())
        << "\n";
 
    report
        << "Confidence            : "
        << recommendation.getConfidence()
        << "%\n";
 
    report
        << "Reason                : "
        << recommendation.getReason()
        << "\n\n";
 
    report
        << "============================================================\n";
 
    report.close();
}
 
void SimulationManager::generateSummaryReport(
    const std::string& applicationName,
    const RecommendationResult& recommendation,
    const ExecutionResult& executionResult,
    const std::vector<BenchmarkResult>& rankedResults
) const
{
    std::ofstream report(
        "reports/summary_report.txt"
    );
 
    if (!report.is_open())
    {
        std::cout
            << "Warning: Unable to create summary report."
            << std::endl;
        return;
    }
 
    report
        << "============================================================\n"
        << "                    SUMMARY REPORT\n"
        << "============================================================\n\n";
 
    report
        << "Application            : "
        << applicationName << "\n\n";
 
    report
        << "--------------- RECOMMENDATION ----------------------------\n\n";
 
    report
        << "Recommended Algorithm  : "
        << algorithmToString(
               recommendation.getRecommendedAlgorithm())
        << "\n";
 
    report
        << std::fixed
        << std::setprecision(2);
 
    report
        << "Confidence             : "
        << recommendation.getConfidence()
        << "%\n";
 
    report
        << "Reason                 : "
        << recommendation.getReason()
        << "\n\n";
 
    report
        << "----------- RECOMMENDED EXECUTION -------------------------\n\n";
 
    report
        << "Algorithm              : "
        << algorithmToString(
               executionResult.getAlgorithm())
        << "\n";
 
    report
        << "Average Waiting Time   : "
        << executionResult.getAverageWaitingTime()
        << "\n";
 
    report
        << "Average Turnaround Time: "
        << executionResult.getAverageTurnaroundTime()
        << "\n";
 
    report
        << "Average Response Time  : "
        << executionResult.getAverageResponseTime()
        << "\n";
 
    report
        << "CPU Utilization        : "
        << executionResult.getCPUUtilization()
        << "%\n";
 
    report
        << "Context Switches       : "
        << executionResult.getContextSwitches()
        << "\n";
 
    report
        << "Preemptions            : "
        << executionResult.getPreemptions()
        << "\n\n";
 
    report
        << "---------------- BENCHMARK RANKING ------------------------\n\n";
 
    for (std::size_t i = 0;
         i < rankedResults.size();
         ++i)
    {
        const BenchmarkResult& result =
            rankedResults[i];
 
        report
            << "Rank "
            << result.getRank()
            << " : "
            << algorithmToString(
                   result.getAlgorithm())
            << " | Score = "
            << result.getRankingScore()
            << "\n";
    }
 
    report << "\n";
 
    if (!rankedResults.empty())
    {
        const AlgorithmType benchmarkWinner =
            rankedResults[0].getAlgorithm();
 
        report
            << "Benchmark Winner       : "
            << algorithmToString(benchmarkWinner)
            << "\n";
 
        if (recommendation.getRecommendedAlgorithm()
            == benchmarkWinner)
        {
            report
                << "Recommendation Result  : "
                << "Matches benchmark winner.\n";
        }
        else
        {
            report
                << "Recommendation Result  : "
                << "Differs from benchmark winner.\n";
 
            report
                << "Observation            : "
                << "The recommendation is workload-based, "
                << "while the benchmark winner is "
                << "performance-based.\n";
        }
    }
 
    report
        << "\n============================================================\n";
 
    report.close();
}

void SimulationManager::generateBenchmarkReport(
    const std::string& applicationName,
    const std::vector<BenchmarkResult>& rankedResults,
    const RecommendationResult& recommendation
) const
{
    std::ofstream report(
        "reports/benchmark_report.txt"
    );
 
    if (!report.is_open())
    {
        std::cout
            << "Warning: Unable to create benchmark report."
            << std::endl;
        return;
    }
 
    report
        << "============================================================\n"
        << "                 BENCHMARK REPORT\n"
        << "============================================================\n\n";
 
    report
        << "Application : "
        << applicationName
        << "\n\n";
 
    report
        << "Recommended Algorithm : "
        << algorithmToString(
               recommendation.getRecommendedAlgorithm())
        << "\n\n";
 
    report
        << "---------------- ALGORITHM COMPARISON ---------------------\n\n";
 
    report
        << std::left
        << std::setw(30) << "Algorithm"
        << std::setw(12) << "Avg WT"
        << std::setw(12) << "Avg TAT"
        << std::setw(12) << "Avg RT"
        << std::setw(12) << "CPU %"
        << std::setw(12) << "Avg CST"
        << std::setw(8)  << "CS"
        << std::setw(10) << "Preempt"
        << std::setw(10) << "Rank"
        << "Score"
        << "\n";
 
    report
        << std::string(128, '-')
        << "\n";
 
    report
        << std::fixed
        << std::setprecision(2);
 
    for (std::size_t i = 0;
         i < rankedResults.size();
         ++i)
    {
        const BenchmarkResult& result =
            rankedResults[i];
 
        report
            << std::left
            << std::setw(30)
            << algorithmToString(
                   result.getAlgorithm())
 
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
 
            << std::setw(8)
            << result.getContextSwitches()
 
            << std::setw(10)
            << result.getPreemptions()
 
            << std::setw(10)
            << result.getRank()
 
            << result.getRankingScore()
            << "\n";
    }
 
    report
        << "\n\n"
        << "---------------- RANKING ----------------\n\n";
 
    for (std::size_t i = 0;
         i < rankedResults.size();
         ++i)
    {
        const BenchmarkResult& result =
            rankedResults[i];
 
        report
            << "Rank "
            << result.getRank()
            << " : "
            << algorithmToString(
                   result.getAlgorithm())
            << " | Score = "
            << result.getRankingScore()
            << "\n";
    }
 
    if (!rankedResults.empty())
    {
        const AlgorithmType winner =
            rankedResults[0].getAlgorithm();
 
        report
            << "\nBenchmark Winner : "
            << algorithmToString(winner)
            << "\n";
 
        report
            << "Recommended      : "
            << algorithmToString(
                   recommendation.getRecommendedAlgorithm())
            << "\n";
 
        if (winner ==
            recommendation.getRecommendedAlgorithm())
        {
            report
                << "Result           : "
                << "Recommendation matches benchmark winner.\n";
        }
        else
        {
            report
                << "Result           : "
                << "Recommendation differs from benchmark winner.\n";
 
            report
                << "Observation      : "
                << "The recommendation is workload-based, "
                << "while the benchmark winner is "
                << "performance-based.\n";
        }
    }
 
    report
        << "\n============================================================\n";
 
    report.close();
}
 
void SimulationManager::generateStateTransitionReport(
    const std::string& applicationName,
    const std::vector<SimulationEvent>& transitions
) const
{
    std::ofstream report(
        "reports/state_transition_report.txt"
    );
 
    if (!report.is_open())
    {
        std::cout
            << "Warning: Unable to create state transition report."
            << std::endl;
        return;
    }
 
    report
        << "============================================================\n"
        << "             STATE TRANSITION REPORT\n"
        << "============================================================\n\n";
 
    report
        << "Application : "
        << applicationName
        << "\n\n";
 
    report
        << "---------------- STATE TRANSITIONS ------------------------\n\n";
 
    report
        << std::left
        << std::setw(30) << "Process"
        << std::setw(10) << "Time"
        << std::setw(15) << "From State"
        << std::setw(15) << "To State"
        << "\n";
 
    report
        << std::string(70, '-')
        << "\n";
 
    for (std::size_t i = 0;
         i < transitions.size();
         ++i)
    {
        const SimulationEvent& event =
            transitions[i];
 
        const StateTransition& transition =
            event.getTransition();
 
        report
            << std::left
            << std::setw(30)
            << event.getProcessName()
            << std::setw(10)
            << event.getTimestamp()
            << std::setw(15)
            << processStateToString(transition.from)
            << std::setw(15)
            << processStateToString(transition.to)
            << "\n";
    }
 
    report
        << "\n---------------- TRANSITION SUMMARY ----------------------\n\n";
 
    int newToReady = 0;
    int readyToRunning = 0;
    int runningToReady = 0;
    int runningToWaiting = 0;
    int waitingToReady = 0;
    int runningToCompleted = 0;
 
    for (std::size_t i = 0;
         i < transitions.size();
         ++i)
    {
        const StateTransition& transition =
            transitions[i].getTransition();
 
        if (transition.from == ProcessState::NEW &&
            transition.to == ProcessState::READY)
        {
            ++newToReady;
        }
        else if (transition.from == ProcessState::READY &&
                 transition.to == ProcessState::RUNNING)
        {
            ++readyToRunning;
        }
        else if (transition.from == ProcessState::RUNNING &&
                 transition.to == ProcessState::READY)
        {
            ++runningToReady;
        }
        else if (transition.from == ProcessState::RUNNING &&
                 transition.to == ProcessState::WAITING)
        {
            ++runningToWaiting;
        }
        else if (transition.from == ProcessState::WAITING &&
                 transition.to == ProcessState::READY)
        {
            ++waitingToReady;
        }
        else if (transition.from == ProcessState::RUNNING &&
                 transition.to == ProcessState::COMPLETED)
        {
            ++runningToCompleted;
        }
    }
 
    report
        << "Total State Transitions : "
        << transitions.size()
        << "\n";
 
    report
        << "NEW -> READY            : "
        << newToReady
        << "\n";
 
    report
        << "READY -> RUNNING        : "
        << readyToRunning
        << "\n";
 
    report
        << "RUNNING -> READY        : "
        << runningToReady
        << "\n";
 
    report
        << "RUNNING -> WAITING      : "
        << runningToWaiting
        << "\n";
 
    report
        << "WAITING -> READY        : "
        << waitingToReady
        << "\n";
 
    report
        << "RUNNING -> COMPLETED    : "
        << runningToCompleted
        << "\n";
 
    report
        << "\n============================================================\n";
 
    report.close();
}
 

void SimulationManager::generateProcessReport(
    const std::vector<ProcessModel>& processes,
    const ExecutionResult& executionResult
) const
{
    std::ofstream report(
        "reports/process_report.csv"
    );
 
    if (!report.is_open())
    {
        std::cout
            << "Warning: Unable to create process report."
            << std::endl;
        return;
    }
 
    report
        << "ProcessName,"
        << "AT,"
        << "BT,"
        << "Priority,"
        << "IOTriggerTime,"
        << "IODuration,"
        << "StartTime,"
        << "CompletionTime,"
        << "WaitingTime,"
        << "TurnaroundTime,"
        << "ResponseTime,"
        << "FinalState"
        << "\n";
 
    const std::vector<ExecutionSlice>& timeline =
        executionResult.getTimeline();
 
    for (std::size_t i = 0;
         i < processes.size();
         ++i)
    {
        const ProcessModel& process = processes[i];
 
        int startTime = -1;
        int completionTime = -1;
 
        for (std::size_t j = 0;
             j < timeline.size();
             ++j)
        {
            const ExecutionSlice& slice = timeline[j];
 
            if (slice.getProcessName() == process.getName())
            {
                if (startTime == -1)
                {
                    startTime =
                        slice.getStartTime();
                }
 
                if (slice.getEndTime() > completionTime)
                {
                    completionTime =
                        slice.getEndTime();
                }
            }
        }
 
        int turnaroundTime = 0;
        int waitingTime = 0;
        int responseTime = 0;
 
        if (startTime >= 0 &&
            completionTime >= 0)
        {
            turnaroundTime =
                completionTime -
                process.getArrivalTime();
 
            waitingTime =
                turnaroundTime -
                process.getBurstTime() -
                process.getIODuration();
 
            if (waitingTime < 0)
                waitingTime = 0;
 
            responseTime =
                startTime -
                process.getArrivalTime();
        }
 
        report
            << process.getName() << ","
            << process.getArrivalTime() << ","
            << process.getBurstTime() << ","
            << process.getOriginalPriority() << ","
            << process.getIOTriggerTime() << ","
            << process.getIODuration() << ","
            << startTime << ","
            << completionTime << ","
            << waitingTime << ","
            << turnaroundTime << ","
            << responseTime << ","
            << "COMPLETED"
            << "\n";
    }
 
    report.close();
}


/*void SimulationManager::runRecommendedAlgorithm(
    const std::vector<ProcessModel>& processes,
    const RecommendationResult& recommendation) const*/
ExecutionResult SimulationManager::runRecommendedAlgorithm(
    const std::vector<ProcessModel>& processes,
    const RecommendationResult& recommendation,
    const std::string& applicationName) const
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

        return ExecutionResult(algorithm);
    }

ExecutionLogger logger;
 
if (!logger.start("logs/execution_log.txt"))
{
    std::cout << "Warning: Unable to open execution log." << std::endl;
}
 
LiveDashboard dashboard(
    applicationName,
    recommendation.getRecommendedAlgorithm(),
    processes,
    250
);
 
scheduler->setObserver(&logger);
scheduler->addObserver(&dashboard);
 
ExecutionResult result =
    scheduler->schedule(processes);
 

generateStateTransitionReport(
    applicationName,
    logger.getStateTransitions()
);

logger.stop();

    MetricsCalculator calculator;

    calculator.calculate(
        processes,
        result
    );

    //displayRecommendedExecution(result);
    displayExecutionTimeline(result);
    //displayGanttChart(result);
 

    std::cout << std::endl;
 
std::cout
    << "========================================================"
    << std::endl;
 
std::cout
    << "           FINAL EXECUTION SUMMARY"
    << std::endl;
 
std::cout
    << "========================================================"
    << std::endl;
 
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
    << "========================================================"
    << std::endl;

return result;
}


void SimulationManager::displayRecommendedExecution(
    const ExecutionResult& result
) const
{
    std::cout << std::endl;
    std::cout << "========================================================"
              << std::endl;
    std::cout << "           FINAL EXECUTION SUMMARY"
              << std::endl;
    std::cout << "========================================================"
              << std::endl;
 
    std::cout
        << "Algorithm                 : "
        << algorithmToString(result.getAlgorithm())
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
 
    std::cout << "========================================================"
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

/*void SimulationManager::displayGanttChart(
    const ExecutionResult& result
) const
{
    const std::vector<ExecutionSlice>& timeline =
        result.getTimeline();

    if (timeline.empty())
    {
        std::cout
            << "No Gantt chart data available."
            << std::endl;

        return;
    }

    std::cout << std::endl;

    std::cout
        << "---------------- GANTT CHART ----------------"
        << std::endl;

    // Process row
    std::cout << "|";

    for (std::size_t i = 0;
         i < timeline.size();
         ++i)
    {
        const ExecutionSlice& slice = timeline[i];

        std::string label =
            " " + slice.getProcessName() + " ";

        std::cout
            << std::left
            << std::setw(12)
            << label
            << "|";
    }

    std::cout << std::endl;

    // Time boundaries
    std::cout << "0";

    for (std::size_t i = 0;
         i < timeline.size();
         ++i)
    {
        const ExecutionSlice& slice = timeline[i];

        std::cout
            << std::right
            << std::setw(12)
            << slice.getEndTime();
    }

    std::cout << std::endl;

    std::cout
        << "----------------------------------------------"
        << std::endl;
}
        */


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
    const std::vector<BenchmarkResult>& rankedResults
)const
{
    if (rankedResults.empty())
    {
        return;
    }
 
    const AlgorithmType recommendedAlgorithm =
        recommendation.getRecommendedAlgorithm();
 
    const AlgorithmType benchmarkWinner =
        rankedResults[0].getAlgorithm();
 
    const BenchmarkResult* recommendedResult = nullptr;
    const BenchmarkResult* winnerResult = nullptr;
 
    for (std::size_t i = 0; i < rankedResults.size(); ++i)
    {
        if (rankedResults[i].getAlgorithm() ==
            recommendedAlgorithm)
        {
            recommendedResult = &rankedResults[i];
        }
 
        if (rankedResults[i].getAlgorithm() ==
            benchmarkWinner)
        {
            winnerResult = &rankedResults[i];
        }
    }
 
    std::cout
        << "\n============================================================\n"
        << "              RECOMMENDATION ANALYSIS\n"
        << "============================================================\n\n";
 
    std::cout
        << "Recommended Algorithm : "
        << algorithmToString(recommendedAlgorithm)
        << "\n";
 
    std::cout
        << "Benchmark Winner      : "
        << algorithmToString(benchmarkWinner)
        << "\n\n";
 
    if (recommendedResult != nullptr &&
        winnerResult != nullptr)
    {
        double waitingDifference =
            recommendedResult->getAverageWaitingTime()
            - winnerResult->getAverageWaitingTime();
 
        double turnaroundDifference =
            recommendedResult->getAverageTurnaroundTime()
            - winnerResult->getAverageTurnaroundTime();
 
        std::cout
            << std::fixed
            << std::setprecision(2);
 
        std::cout
            << "Recommended Avg WT    : "
            << recommendedResult->getAverageWaitingTime()
            << "\n";
 
        std::cout
            << "Benchmark Winner WT   : "
            << winnerResult->getAverageWaitingTime()
            << "\n";
 
        std::cout
            << "WT Difference         : "
            << waitingDifference
            << "\n\n";
 
        std::cout
            << "Recommended Avg TAT   : "
            << recommendedResult->getAverageTurnaroundTime()
            << "\n";
 
        std::cout
            << "Benchmark Winner TAT  : "
            << winnerResult->getAverageTurnaroundTime()
            << "\n";
 
        std::cout
            << "TAT Difference        : "
            << turnaroundDifference
            << "\n\n";
    }
 
    if (recommendedAlgorithm == benchmarkWinner)
    {
        std::cout
            << "Result                : "
            << "Recommendation matches benchmark winner.\n";
 
        std::cout
            << "Observation           : "
            << "The workload-based recommendation "
            << "also achieved the best benchmark score.\n";
    }
    else
    {
        std::cout
            << "Result                : "
            << "Recommendation differs from benchmark winner.\n";
 
        std::cout
            << "Observation           : "
            << "The recommendation is workload-based, "
            << "while the benchmark winner is "
            << "performance-based.\n";
    }
 
    std::cout
        << "\n============================================================\n";
}
 
/*void SimulationManager::displayBenchmarkResults(
    const std::vector<BenchmarkResult>& results
) const
{
    std::cout << "\n------------- BENCHMARK -----------------\n";
 
    std::cout << std::left
              << std::setw(32) << "Algorithm"
              << std::setw(11) << "Avg WT"
              << std::setw(12) << "Avg TAT"
              << std::setw(12) << "Avg RT"
              << std::setw(12) << "CPU %"
              << std::setw(14) << "Avg CST"
              << std::setw(10) << "CS"
              << "Preempt\n";
 
    for (std::size_t i = 0; i < results.size(); ++i)
    {
        const BenchmarkResult& result = results[i];
 
        std::cout << std::left
                  << std::setw(32) << algorithmToString(result.getAlgorithm())
                  << std::setw(11) << std::fixed << std::setprecision(2)
                  << result.getAverageWaitingTime()
                  << std::setw(12) << result.getAverageTurnaroundTime()
                  << std::setw(12) << result.getAverageResponseTime()
                  << std::setw(12) << result.getCpuUtilization()
                  << std::setw(14) << result.getAverageContextSwitchTime()
                  << std::setw(10) << result.getContextSwitches()
                  << result.getPreemptions()
                  << "\n";
    }
}*/

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
