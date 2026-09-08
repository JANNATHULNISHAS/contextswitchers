#include "WorkloadAnalyzer.h"
 
#include <algorithm>
#include <cmath>
#include <set>
 
WorkloadMetrics WorkloadAnalyzer::analyze(
    const std::vector<ProcessModel>& processes
) const
{
    WorkloadMetrics metrics;
 
    if (processes.empty())
    {
        return metrics;
    }
 
    const int processCount =
        static_cast<int>(processes.size());
 
    metrics.setProcessCount(processCount);
 
    // Burst-time statistics
    double totalBurst = 0.0;
    double minimumBurst = processes[0].getBurstTime();
    double maximumBurst = processes[0].getBurstTime();
 
    for (std::size_t i = 0; i < processes.size(); ++i)
    {
        const int burst = processes[i].getBurstTime();
 
        totalBurst += burst;
 
        minimumBurst =
            std::min(minimumBurst, static_cast<double>(burst));
 
        maximumBurst =
            std::max(maximumBurst, static_cast<double>(burst));
    }
 
    const double averageBurst =
        totalBurst / processCount;
 
    metrics.setAverageBurstTime(averageBurst);
    metrics.setMinimumBurstTime(minimumBurst);
    metrics.setMaximumBurstTime(maximumBurst);
 
    // Population variance
    double variance = 0.0;
 
    for (std::size_t i = 0; i < processes.size(); ++i)
    {
        const double difference =
            processes[i].getBurstTime() - averageBurst;
 
        variance += difference * difference;
    }
 
    variance /= processCount;
 
    metrics.setBurstVariance(variance);
 
    // Variance classification
    if (variance < 20.0)
    {
        metrics.setVarianceLevel(VarianceLevel::LOW);
    }
    else if (variance < 100.0)
    {
        metrics.setVarianceLevel(VarianceLevel::MEDIUM);
    }
    else
    {
        metrics.setVarianceLevel(VarianceLevel::HIGH);
    }
 
    // Priority statistics
    std::set<int> priorityLevels;
    int highestPriorityCount = 0;
 
    for (std::size_t i = 0; i < processes.size(); ++i)
    {
        const int priority =
            processes[i].getOriginalPriority();
 
        priorityLevels.insert(priority);
 
        if (priority == 1)
        {
            ++highestPriorityCount;
        }
    }
 
    metrics.setPriorityLevelCount(
        static_cast<int>(priorityLevels.size())
    );
 
    metrics.setHighestPriorityProcessCount(
        highestPriorityCount
    );
 
    // I/O statistics
    int ioProcessCount = 0;
 
    for (std::size_t i = 0; i < processes.size(); ++i)
    {
        if (processes[i].hasIO())
        {
            ++ioProcessCount;
        }
    }
 
    metrics.setIOProcessCount(ioProcessCount);
 
    // Arrival pattern
    bool allArriveAtZero = true;
 
    for (std::size_t i = 0; i < processes.size(); ++i)
    {
        if (processes[i].getArrivalTime() != 0)
        {
            allArriveAtZero = false;
            break;
        }
    }
 
    if (allArriveAtZero)
    {
        metrics.setArrivalPattern(ArrivalPattern::BATCH);
    }
    else if (processCount > 10)
    {
        metrics.setArrivalPattern(ArrivalPattern::INTERACTIVE);
    }
    else
    {
        metrics.setArrivalPattern(ArrivalPattern::MIXED);
    }
 
    // Overall workload classification
    if (highestPriorityCount >= 3)
    {
        metrics.setWorkloadType(
            WorkloadType::PRIORITY_CRITICAL
        );
    }
    else if (
        metrics.getArrivalPattern() ==
        ArrivalPattern::INTERACTIVE
    )
    {
        metrics.setWorkloadType(
            WorkloadType::INTERACTIVE
        );
    }
    else if (
        metrics.getArrivalPattern() ==
        ArrivalPattern::BATCH
    )
    {
        metrics.setWorkloadType(
            WorkloadType::BATCH
        );
    }
    else
    {
        metrics.setWorkloadType(
            WorkloadType::MIXED
        );
    }
 
    return metrics;
}
 
