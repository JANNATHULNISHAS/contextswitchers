#include "BenchmarkEngine.h"
 
#include <thread>
#include <vector>
 
#include "SchedulerFactory.h"
#include "MetricsCalculator.h"
 
std::vector<BenchmarkResult>
BenchmarkEngine::benchmark(
    const std::vector<ProcessModel>& processes) const
{
    std::vector<BenchmarkResult> results;
 
    if (processes.empty())
    {
        return results;
    }
 
    /*
     * Six scheduling algorithms/modes.
     */
    const AlgorithmType algorithms[] =
    {
        AlgorithmType::FCFS,
        AlgorithmType::SJF,
        AlgorithmType::SRTF,
        AlgorithmType::PRIORITY_NON_PREEMPTIVE,
        AlgorithmType::PRIORITY_PREEMPTIVE,
        AlgorithmType::ROUND_ROBIN
    };
 
    const int algorithmCount =
        sizeof(algorithms) / sizeof(algorithms[0]);
 
    /*
     * One result slot per algorithm.
     *
     * Each thread writes only to its own result slot,
     * so no shared result object is modified.
     */
    std::vector<BenchmarkResult> threadResults;
 
    for (int i = 0; i < algorithmCount; ++i)
    {
        threadResults.push_back(
            BenchmarkResult(algorithms[i])
        );
    }
 
    std::vector<std::thread> workers;
 
    /*
     * Run each algorithm independently.
     *
     * Only BenchmarkEngine creates threads.
     */
    for (int i = 0; i < algorithmCount; ++i)
    {
        workers.push_back(
            std::thread(
                [this, &processes, &threadResults, &algorithms, i]()
                {
                    threadResults[i] =
                        benchmarkAlgorithm(
                            processes,
                            algorithms[i]
                        );
                }
            )
        );
    }
 
    /*
     * Wait for all benchmark threads.
     */
    for (std::size_t i = 0; i < workers.size(); ++i)
    {
        workers[i].join();
    }
 
    /*
     * Return results in the same deterministic order
     * as the algorithm list.
     */
    for (int i = 0; i < algorithmCount; ++i)
    {
        results.push_back(threadResults[i]);
    }
 
    return results;
}
 
BenchmarkResult
BenchmarkEngine::benchmarkAlgorithm(
    const std::vector<ProcessModel>& processes,
    AlgorithmType algorithm) const
{
    BenchmarkResult benchmarkResult(algorithm);
 
    /*
     * Factory creates an independent scheduler instance.
     */
    std::unique_ptr<Scheduler> scheduler =
        SchedulerFactory::create(algorithm);
 
    if (!scheduler)
    {
        return benchmarkResult;
    }
 
    /*
     * Each scheduler receives the original workload
     * as const input and internally works on its own copy.
     */
    ExecutionResult executionResult =
        scheduler->schedule(processes);
 
    /*
     * Calculate performance metrics from the execution
     * timeline.
     */
    MetricsCalculator calculator;
 
    calculator.calculate(
        processes,
        executionResult
    );
 
    benchmarkResult.setAverageWaitingTime(
        executionResult.getAverageWaitingTime()
    );
 
    benchmarkResult.setAverageTurnaroundTime(
        executionResult.getAverageTurnaroundTime()
    );
    
    benchmarkResult.setAverageResponseTime(
        executionResult.getAverageResponseTime()
    );
 
    benchmarkResult.setThroughput(
        executionResult.getThroughput()
    );
 
    benchmarkResult.setCPUUtilization(
        executionResult.getCPUUtilization()
    );
    benchmarkResult.setAverageContextSwitchTime(
        executionResult.getAverageContextSwitchTime()
    );
    benchmarkResult.setContextSwitches(
        executionResult.getContextSwitches()
    );
 
    benchmarkResult.setPreemptions(
        executionResult.getPreemptions()
    );
 
    return benchmarkResult;
}
 
