#include "PriorityScheduler.h"
 
#include <vector>
#include <string>
 
ExecutionResult PriorityScheduler::schedule(
    const std::vector<ProcessModel>& inputProcesses)
{
    std::vector<ProcessModel> processes = inputProcesses;
 
    ExecutionResult result(
        AlgorithmType::PRIORITY_NON_PREEMPTIVE
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
 
    int currentProcess = -1;
 
    int time = 0;
    int busyTime = 0;
    int idleTime = 0;
    int contextSwitches = 0;
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
            }
        }
 
        /*
         * Move processes from WAITING to READY
         * after their complete I/O duration.
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
 
                ioEndTime[i] = -1;
            }
        }
 
        /*
         * Select the READY process with the
         * highest priority.
         *
         * Lower priority number = higher priority.
         */
        if (currentProcess == -1)
        {
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
 
            if (selectedProcess != -1)
            {
                currentProcess = selectedProcess;
 
                ProcessModel& process =
                    processes[currentProcess];
 
                process.changeState(
                    ProcessState::RUNNING
                );
 
                if (process.getStartTime() == -1)
                {
                    process.setStartTime(time);
                }
 
                const std::string& currentName =
                    process.getName();
 
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
 
        ProcessModel& process =
            processes[currentProcess];
 
        const int sliceStart = time;
 
        /*
         * Execute one CPU time unit.
         */
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
                time + process.getIODuration();
 
            currentProcess = -1;
        }
    }
 
    result.setTotalExecutionTime(time);
    result.setCPUBusyTime(busyTime);
    result.setCPUIdleTime(idleTime);
    result.setContextSwitches(contextSwitches);
 
    return result;
}
 
AlgorithmType
PriorityScheduler::getAlgorithmType() const
{
    return AlgorithmType::PRIORITY_NON_PREEMPTIVE;
}
 

 
