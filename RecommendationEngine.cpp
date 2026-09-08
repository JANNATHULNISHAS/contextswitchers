#include "RecommendationEngine.h"
 
RecommendationResult
RecommendationEngine::recommend(
    const WorkloadMetrics& metrics) const
{
    const VarianceLevel variance =
        metrics.getVarianceLevel();
 
    const ArrivalPattern pattern =
        metrics.getArrivalPattern();
 
    const WorkloadType workloadType =
        metrics.getWorkloadType();
 
    const int processCount =
        metrics.getProcessCount();
 
    const int priorityLevels =
        metrics.getPriorityLevelCount();
 
    const int highestPriorityCount =
        metrics.getHighestPriorityProcessCount();
 
    /*
     * Rule 1:
     * Priority-critical workloads should use
     * Preemptive Priority Scheduling.
     *
     * Lower priority number = higher priority.
     */
    if (workloadType == WorkloadType::PRIORITY_CRITICAL ||
        highestPriorityCount >= 3)
    {
        return recommendPriority(metrics);
    }
 
    /*
     * Rule 2:
     * Large interactive workloads benefit from
     * Round Robin because of fairness.
     */
    if (processCount > 20 &&
        pattern == ArrivalPattern::INTERACTIVE)
    {
        return recommendRoundRobin(metrics);
    }
 
    /*
     * Rule 3:
     * Interactive workload with highly varying
     * burst times benefits from SRTF.
     */
    if (variance == VarianceLevel::HIGH &&
        pattern == ArrivalPattern::INTERACTIVE)
    {
        return recommendSRTF(metrics);
    }
 
    /*
     * Rule 4:
     * Batch workload with highly varying burst
     * times can benefit from SJF.
     */
    if (variance == VarianceLevel::HIGH &&
        pattern == ArrivalPattern::BATCH)
    {
        return recommendSJF(metrics);
    }
 
    /*
     * Rule 5:
     * Multiple priority levels indicate that
     * priority-aware scheduling may be useful.
     */
    if (priorityLevels >= 4)
    {
        return recommendPriority(metrics);
    }
 
    /*
     * Default:
     * FCFS is selected for simple workloads.
     */
    return recommendFCFS(metrics);
}
 
 
/*
* FCFS Recommendation
*/
RecommendationResult
RecommendationEngine::recommendFCFS(
    const WorkloadMetrics&) const
{
    return RecommendationResult(
        AlgorithmType::FCFS,
        "Simple workload detected. FCFS provides straightforward and predictable scheduling.",
        75.0
    );
}
 
 
/*
* SJF Recommendation
*/
RecommendationResult
RecommendationEngine::recommendSJF(
    const WorkloadMetrics&) const
{
    return RecommendationResult(
        AlgorithmType::SJF,
        "High burst-time variance in a batch workload makes SJF suitable for reducing average waiting time.",
        85.0
    );
}
 
 
/*
* SRTF Recommendation
*/
RecommendationResult
RecommendationEngine::recommendSRTF(
    const WorkloadMetrics&) const
{
    return RecommendationResult(
        AlgorithmType::SRTF,
        "High burst-time variance with interactive arrivals makes SRTF suitable for improving response and waiting time.",
        92.0
    );
}
 
 
/*
* Priority Recommendation
*
* Default priority mode:
* PREEMPTIVE
*/
RecommendationResult
RecommendationEngine::recommendPriority(
    const WorkloadMetrics&) const
{
    return RecommendationResult(
        AlgorithmType::PRIORITY_PREEMPTIVE,
        "Priority-critical workload detected. Preemptive Priority Scheduling is recommended so higher-priority processes can receive CPU service immediately.",
        90.0
    );
}
 
 
/*
* Round Robin Recommendation
*/
RecommendationResult
RecommendationEngine::recommendRoundRobin(
    const WorkloadMetrics&) const
{
    return RecommendationResult(
        AlgorithmType::ROUND_ROBIN,
        "Large interactive workload detected. Round Robin provides fair CPU allocation among processes.",
        88.0
    );
}
