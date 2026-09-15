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

    // --------------------------------------------------
    // Burst-time statistics
    // --------------------------------------------------

    double totalBurst = 0.0;

    double minimumBurst =
        processes[0].getBurstTime();

    double maximumBurst =
        processes[0].getBurstTime();

    for (std::size_t i = 0;
         i < processes.size();
         ++i)
    {
        const int burst =
            processes[i].getBurstTime();

        totalBurst += burst;

        minimumBurst =
            std::min(
                minimumBurst,
                static_cast<double>(burst)
            );

        maximumBurst =
            std::max(
                maximumBurst,
                static_cast<double>(burst)
            );
    }

    const double averageBurst =
        totalBurst / processCount;

    metrics.setAverageBurstTime(
        averageBurst
    );

    metrics.setMinimumBurstTime(
        minimumBurst
    );

    metrics.setMaximumBurstTime(
        maximumBurst
    );

    // --------------------------------------------------
    // Population variance
    // --------------------------------------------------

    double variance = 0.0;

    for (std::size_t i = 0;
         i < processes.size();
         ++i)
    {
        const double difference =
            processes[i].getBurstTime()
            - averageBurst;

        variance +=
            difference * difference;
    }

    variance /= processCount;

    metrics.setBurstVariance(
        variance
    );

    // --------------------------------------------------
    // Variance classification
    //
    // LOW    : variance < 20
    // MEDIUM : variance < 50
    // HIGH   : variance >= 50
    // --------------------------------------------------

    if (variance < 20.0)
    {
        metrics.setVarianceLevel(
            VarianceLevel::LOW
        );
    }
    else if (variance < 50.0)
    {
        metrics.setVarianceLevel(
            VarianceLevel::MEDIUM
        );
    }
    else
    {
        metrics.setVarianceLevel(
            VarianceLevel::HIGH
        );
    }

    // --------------------------------------------------
    // Priority statistics
    // --------------------------------------------------

    std::set<int> priorityLevels;

    int highestPriorityCount = 0;

    for (std::size_t i = 0;
         i < processes.size();
         ++i)
    {
        const int priority =
            processes[i].getOriginalPriority();

        priorityLevels.insert(
            priority
        );

        /*
         * Priority 1 is the highest priority.
         */
        if (priority == 1)
        {
            ++highestPriorityCount;
        }
    }

    metrics.setPriorityLevelCount(
        static_cast<int>(
            priorityLevels.size()
        )
    );

    metrics.setHighestPriorityProcessCount(
        highestPriorityCount
    );

    // --------------------------------------------------
    // I/O statistics
    // --------------------------------------------------

    int ioProcessCount = 0;

    for (std::size_t i = 0;
         i < processes.size();
         ++i)
    {
        if (processes[i].hasIO())
        {
            ++ioProcessCount;
        }
    }

    metrics.setIOProcessCount(
        ioProcessCount
    );

    // --------------------------------------------------
    // Arrival pattern
    //
    // BATCH:
    //     All processes arrive at time 0.
    //
    // INTERACTIVE:
    //     Processes arrive over time and the workload
    //     contains meaningful I/O activity.
    //
    // MIXED:
    //     Processes arrive over time, but the workload
    //     does not have enough I/O activity to classify
    //     it as interactive.
    //
    // IMPORTANT:
    //     Process count is NOT used here.
    // --------------------------------------------------

    bool allArriveAtZero = true;

    int minimumArrivalTime =
        processes[0].getArrivalTime();

    int maximumArrivalTime =
        processes[0].getArrivalTime();

    for (std::size_t i = 0;
         i < processes.size();
         ++i)
    {
        const int arrivalTime =
            processes[i].getArrivalTime();

        if (arrivalTime != 0)
        {
            allArriveAtZero = false;
        }

        minimumArrivalTime =
            std::min(
                minimumArrivalTime,
                arrivalTime
            );

        maximumArrivalTime =
            std::max(
                maximumArrivalTime,
                arrivalTime
            );
    }

    /*
     * Calculate the arrival spread.
     */
    const int arrivalSpread =
        maximumArrivalTime -
        minimumArrivalTime;

    /*
     * A workload with staggered arrivals has
     * processes entering the system over time.
     */
    const bool staggeredArrivals =
        arrivalSpread > 0;

    /*
     * At least two I/O processes means the workload
     * has repeated CPU/WAITING interaction.
     */
    const bool interactiveIO =
        ioProcessCount >= 2;

    if (allArriveAtZero)
    {
        metrics.setArrivalPattern(
            ArrivalPattern::BATCH
        );
    }
    else if (staggeredArrivals &&
             interactiveIO)
    {
        metrics.setArrivalPattern(
            ArrivalPattern::INTERACTIVE
        );
    }
    else
    {
        metrics.setArrivalPattern(
            ArrivalPattern::MIXED
        );
    }

    // --------------------------------------------------
    // Overall workload classification
    // --------------------------------------------------

    /*
     * Three or more priority-1 processes indicate
     * a priority-critical workload.
     */
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
