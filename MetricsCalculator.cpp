#include "MetricsCalculator.h"

#include <map>
#include <string>

void MetricsCalculator::calculate(
    const std::vector<ProcessModel>& processes,
    ExecutionResult& result
) const
{
    if (processes.empty())
    {
        return;
    }

    const std::vector<ExecutionSlice>& timeline =
        result.getTimeline();

    if (timeline.empty())
    {
        return;
    }

    std::map<std::string, int> firstStartTime;
    std::map<std::string, int> completionTime;

    /*
     * Find the first CPU execution time and
     * final CPU execution time of each process.
     */
    for (std::size_t i = 0; i < timeline.size(); ++i)
    {
        const ExecutionSlice& slice = timeline[i];

        const std::string& name =
            slice.getProcessName();

        if (firstStartTime.find(name) ==
            firstStartTime.end())
        {
            firstStartTime[name] =
                slice.getStartTime();
        }

        completionTime[name] =
            slice.getEndTime();
    }

    double totalWaitingTime = 0.0;
    double totalTurnaroundTime = 0.0;
    double totalResponseTime = 0.0;

    int validProcessCount = 0;

    for (std::size_t i = 0; i < processes.size(); ++i)
    {
        const ProcessModel& process = processes[i];

        const std::string& name =
            process.getName();

        if (firstStartTime.find(name) ==
                firstStartTime.end() ||
            completionTime.find(name) ==
                completionTime.end())
        {
            continue;
        }

        const int startTime =
            firstStartTime[name];

        const int completedAt =
            completionTime[name];

        /*
         * Turnaround Time
         *
         * TAT = Completion Time - Arrival Time
         */
        const int turnaroundTime =
            completedAt -
            process.getArrivalTime();

        /*
         * Response Time
         *
         * RT = First CPU Start - Arrival Time
         */
        const int responseTime =
            startTime -
            process.getArrivalTime();

        /*
         * Waiting Time
         *
         * WT = TAT - CPU Burst - I/O Time
         *
         * I/O is subtracted because time spent in
         * WAITING is not CPU ready-queue waiting.
         */
        int ioTime = 0;

        if (process.hasIO() &&
            process.isIOCompleted())
        {
            ioTime =
                process.getIODuration();
        }

        int waitingTime =
            turnaroundTime -
            process.getBurstTime() -
            ioTime;

        /*
         * Protect against invalid negative values.
         */
        if (waitingTime < 0)
        {
            waitingTime = 0;
        }

        totalWaitingTime += waitingTime;
        totalTurnaroundTime += turnaroundTime;
        totalResponseTime += responseTime;

        ++validProcessCount;
    }

    /*
     * Average process metrics.
     */
    if (validProcessCount > 0)
    {
        result.setAverageWaitingTime(
            totalWaitingTime /
            validProcessCount
        );

        result.setAverageTurnaroundTime(
            totalTurnaroundTime /
            validProcessCount
        );

        result.setAverageResponseTime(
            totalResponseTime /
            validProcessCount
        );
    }

    /*
     * Context Switch Analytics
     *
     * A context switch occurs when consecutive
     * execution slices belong to different processes.
     */
    int contextSwitchCount = 0;

    for (std::size_t i = 1; i < timeline.size(); ++i)
    {
        const ExecutionSlice& previous =
            timeline[i - 1];

        const ExecutionSlice& current =
            timeline[i];

        if (previous.getProcessName() !=
            current.getProcessName())
        {
            ++contextSwitchCount;
        }
    }

    result.setContextSwitches(
        contextSwitchCount
    );

    /*
     * No explicit context-switch overhead is
     * currently simulated.
     */
    result.setAverageContextSwitchTime(0.0);

    /*
     * CPU utilization and throughput.
     */
    const int totalExecutionTime =
        result.getTotalExecutionTime();

    if (totalExecutionTime > 0)
    {
        /*
         * Throughput =
         * Number of completed processes / total time
         */
        result.setThroughput(
            static_cast<double>(validProcessCount) /
            totalExecutionTime
        );

        /*
         * CPU Utilization =
         * CPU Busy Time / Total Execution Time * 100
         */
        result.setCPUUtilization(
            (
                static_cast<double>(
                    result.getCPUBusyTime()
                ) /
                totalExecutionTime
            ) * 100.0
        );
    }
}
