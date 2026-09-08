#include "RoundRobinScheduler.h"
 
#include <queue>
#include <vector>
#include <string>
 
RoundRobinScheduler::RoundRobinScheduler(int quantum)
    : timeQuantum(quantum)
{
}
 
ExecutionResult RoundRobinScheduler::schedule(
    const std::vector<ProcessModel>& inputProcesses)
{
    std::vector<ProcessModel> processes =
        inputProcesses;
 
    ExecutionResult result(
        AlgorithmType::ROUND_ROBIN
    );
 
    const std::size_t processCount =
        processes.size();
 
    if (processCount == 0)
    {
        return result;
    }
 
    std::queue<std::size_t> readyQueue;
 
    std::vector<bool> admitted(
        processCount,
        false
    );
 
    std::vector<int> ioEndTime(
        processCount,
        -1
    );
 
    int currentProcess = -1;
    int quantumUsed = 0;
 
    int time = 0;
    int busyTime = 0;
    int idleTime = 0;
    int contextSwitches = 0;
    int preemptions = 0;
    int completedCount = 0;
 
    std::string previousProcess = "";
 
    while (completedCount <
           static_cast<int>(processCount))
    {
        /*
         * Admit newly arrived processes.
         */
        for (std::size_t i = 0;
             i < processCount;
             ++i)
        {
            if (!admitted[i] &&
                processes[i].getArrivalTime() <= time)
            {
                admitted[i] = true;
 
                processes[i].changeState(
                    ProcessState::READY
                );
 
                readyQueue.push(i);
            }
        }
 
        /*
         * Complete I/O operations.
         *
         * I/O is triggered only according to
         * the values provided in the CSV file.
         */
        for (std::size_t i = 0;
             i < processCount;
             ++i)
        {
            if (processes[i].getState() ==
                    ProcessState::WAITING &&
                ioEndTime[i] != -1 &&
                time >= ioEndTime[i])
            {
                processes[i].changeState(
                    ProcessState::READY
                );
 
                readyQueue.push(i);
 
                ioEndTime[i] = -1;
            }
        }
 
        /*
         * Select the next process when CPU is free.
         */
        if (currentProcess == -1)
        {
            if (!readyQueue.empty())
            {
                currentProcess =
                    static_cast<int>(
                        readyQueue.front()
                    );
 
                readyQueue.pop();
 
                quantumUsed = 0;
 
                processes[currentProcess].changeState(
                    ProcessState::RUNNING
                );
 
                /*
                 * Record first CPU start time.
                 */
                if (processes[currentProcess]
                        .getStartTime() == -1)
                {
                    processes[currentProcess]
                        .setStartTime(time);
                }
 
                /*
                 * A context switch occurs when
                 * CPU changes from one process to another.
                 */
                const std::string& currentName =
                    processes[currentProcess]
                        .getName();
 
                if (!previousProcess.empty() &&
                    previousProcess != currentName)
                {
                    ++contextSwitches;
                }
 
                previousProcess = currentName;
            }
            else
            {
                /*
                 * No process is ready.
                 * CPU remains idle for one unit.
                 */
                ++idleTime;
                ++time;
 
                continue;
            }
        }
 
        /*
         * Execute one CPU time unit.
         */
        ProcessModel& process =
            processes[currentProcess];
 
        const int sliceStart = time;
 
        process.executeOneUnit();
 
        ++busyTime;
        ++time;
        ++quantumUsed;
 
        /*
         * Record execution on the timeline.
         */
        result.addExecutionSlice(
            ExecutionSlice(
                process.getName(),
                sliceStart,
                time
            )
        );
 
        /*
         * Process completed.
         *
         * Completion is checked before quantum expiry,
         * so a process finishing exactly at the quantum
         * boundary is NOT counted as a preemption.
         */
        if (process.getRemainingTime() == 0)
        {
            process.changeState(
                ProcessState::COMPLETED
            );
 
            process.setCompletionTime(time);
 
            result.addCompletedProcess(
                process.getName()
            );
 
            ++completedCount;
 
            currentProcess = -1;
            quantumUsed = 0;
 
            continue;
        }
 
        /*
         * CSV-defined I/O trigger.
         *
         * I/O takes priority over quantum expiration.
         * If the process reaches its I/O trigger,
         * it enters WAITING rather than being
         * counted as a Round Robin preemption.
         */
        if (process.hasIO() &&
            !process.isIOCompleted() &&
            process.getExecutedTime() >=
                process.getIOTriggerTime())
        {
            process.markIOCompleted();
 
            process.changeState(
                ProcessState::WAITING
            );
 
            ioEndTime[currentProcess] =
                time +
                process.getIODuration();
 
            currentProcess = -1;
            quantumUsed = 0;
 
            continue;
        }
 
        /*
         * Round Robin time quantum expired.
         *
         * If the process still has remaining CPU burst,
         * it is forcibly removed from the CPU.
         *
         * Therefore this is counted as a PREEMPTION.
         *
         * The context switch itself is counted later
         * when another process is actually selected.
         */
        if (quantumUsed >= timeQuantum)
        {
            process.changeState(
                ProcessState::READY
            );
 
            readyQueue.push(
                static_cast<std::size_t>(
                    currentProcess
                )
            );
 
            /*
             * Quantum expiration with remaining work
             * is a Round Robin preemption.
             */
            ++preemptions;
 
            /*
             * Do NOT increment contextSwitches here.
             * The actual process-to-process switch is
             * counted when the next process is selected.
             */
            currentProcess = -1;
            quantumUsed = 0;
        }
    }
 
    /*
     * Store final execution statistics.
     */
    result.setTotalExecutionTime(time);
 
    result.setCPUBusyTime(busyTime);
 
    result.setCPUIdleTime(idleTime);
 
    result.setContextSwitches(
        contextSwitches
    );
 
    result.setPreemptions(
        preemptions
    );
 
    return result;
}
 
AlgorithmType
RoundRobinScheduler::getAlgorithmType() const
{
    return AlgorithmType::ROUND_ROBIN;
}
int RoundRobinScheduler::getTimeQuantum() const
{
    return timeQuantum;
}
