#include "PreemptivePriorityScheduler.h"
 
#include <vector>
#include <string>
 
ExecutionResult PreemptivePriorityScheduler::schedule(
    const std::vector<ProcessModel>& inputProcesses)
{
    std::vector<ProcessModel> processes = inputProcesses;
 
    ExecutionResult result(
        AlgorithmType::PRIORITY_PREEMPTIVE
    );
 
    const std::size_t processCount =
        processes.size();
 
    if (processCount == 0)
    {
        return result;
    }
 
    std::vector<bool> admitted(
        processCount,
        false
    );
 
    std::vector<int> ioEndTime(
        processCount,
        -1
    );
 
    /*
     * Time at which each process entered READY.
     * Used to implement the 5-time-unit aging rule.
     */
    std::vector<int> readySince(
        processCount,
        -1
    );
 
    int currentProcess = -1;
 
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
 
                readySince[i] = time;
            }
        }
 
        /*
         * Complete I/O operations.
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
 
                readySince[i] = time;
 
                ioEndTime[i] = -1;
            }
        }
 
        /*
         * Apply aging.
         *
         * Every 5 simulation time units spent
         * continuously in READY:
         *
         * effective priority = effective priority - 1
         *
         * until priority reaches 1.
         *
         * Original priority is never changed.
         */
        for (std::size_t i = 0;
             i < processCount;
             ++i)
        {
            if (processes[i].getState() ==
                    ProcessState::READY &&
                readySince[i] != -1)
            {
                const int readyTime =
                    time - readySince[i];
 
                if (readyTime >= AGING_THRESHOLD)
                {
                    const int agingEvents =
                        readyTime /
                        AGING_THRESHOLD;
 
                    const int alreadyApplied =
                        processes[i].getAgingEvents();
 
                    const int newEvents =
                        agingEvents -
                        alreadyApplied;
 
                    for (int j = 0;
                         j < newEvents;
                         ++j)
                    {
                        processes[i]
                            .improveEffectivePriority();
 
                        processes[i]
                            .incrementAgingEvents();
                    }
                }
            }
        }
 
        /*
         * Find the READY process with the
         * highest effective priority.
         *
         * Lower number = higher priority.
         */
        int selectedProcess = -1;
 
        for (std::size_t i = 0;
             i < processCount;
             ++i)
        {
            if (processes[i].getState() !=
                ProcessState::READY)
            {
                continue;
            }
 
            if (selectedProcess == -1)
            {
                selectedProcess =
                    static_cast<int>(i);
 
                continue;
            }
 
            if (processes[i].getEffectivePriority() <
                processes[selectedProcess]
                    .getEffectivePriority())
            {
                selectedProcess =
                    static_cast<int>(i);
            }
            else if (
                processes[i].getEffectivePriority() ==
                processes[selectedProcess]
                    .getEffectivePriority() &&
                processes[i].getArrivalTime() <
                processes[selectedProcess]
                    .getArrivalTime())
            {
                selectedProcess =
                    static_cast<int>(i);
            }
        }
 
        /*
         * Preemption check.
         */
        if (currentProcess != -1)
        {
            if (selectedProcess != -1 &&
                processes[selectedProcess]
                    .getEffectivePriority() <
                processes[currentProcess]
                    .getEffectivePriority())
            {
                processes[currentProcess].changeState(
                    ProcessState::READY
                );
 
                readySince[currentProcess] =
                    time;
 
                processes[currentProcess]
                    .incrementPreemptionCount();
 
                ++preemptions;
                ++contextSwitches;
 
                currentProcess =
                    selectedProcess;
 
                processes[currentProcess].changeState(
                    ProcessState::RUNNING
                );
 
                readySince[currentProcess] = -1;
 
                if (processes[currentProcess]
                        .getStartTime() == -1)
                {
                    processes[currentProcess]
                        .setStartTime(time);
                }
 
                previousProcess =
                    processes[currentProcess]
                        .getName();
            }
        }
        else
        {
            /*
             * CPU is free.
             */
            if (selectedProcess != -1)
            {
                currentProcess =
                    selectedProcess;
 
                processes[currentProcess].changeState(
                    ProcessState::RUNNING
                );
 
                readySince[currentProcess] = -1;
 
                if (processes[currentProcess]
                        .getStartTime() == -1)
                {
                    processes[currentProcess]
                        .setStartTime(time);
                }
 
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
 
        result.addExecutionSlice(
            ExecutionSlice(
                process.getName(),
                sliceStart,
                time
            )
        );
 
        /*
         * Process completed.
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
 
            continue;
        }
 
        /*
         * CSV-defined I/O trigger.
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
        }
    }
 
    result.setTotalExecutionTime(time);
    result.setCPUBusyTime(busyTime);
    result.setCPUIdleTime(idleTime);
    result.setContextSwitches(contextSwitches);
    result.setPreemptions(preemptions);
 
    return result;
}
 
AlgorithmType
PreemptivePriorityScheduler::getAlgorithmType() const
{
    return AlgorithmType::PRIORITY_PREEMPTIVE;
}
 
