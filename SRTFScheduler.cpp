#include "SRTFScheduler.h"
 
#include <vector>
#include <string>
 
ExecutionResult SRTFScheduler::schedule(
    const std::vector<ProcessModel>& inputProcesses)
{
    std::vector<ProcessModel> processes = inputProcesses;
 
    ExecutionResult result(AlgorithmType::SRTF);
 
    const std::size_t processCount = processes.size();
 
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
 
                ioEndTime[i] = -1;
            }
        }
 
        /*
         * Find the READY process with the
         * shortest remaining time.
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
 
            if (processes[i].getRemainingTime() <
                processes[selectedProcess]
                    .getRemainingTime())
            {
                selectedProcess =
                    static_cast<int>(i);
            }
            else if (
                processes[i].getRemainingTime() ==
                processes[selectedProcess]
                    .getRemainingTime() &&
                processes[i].getArrivalTime() <
                processes[selectedProcess]
                    .getArrivalTime())
            {
                selectedProcess =
                    static_cast<int>(i);
            }
        }
 
        /*
         * Decide whether the current process
         * should continue or be preempted.
         */
        if (currentProcess != -1)
        {
            if (selectedProcess != -1 &&
                processes[selectedProcess]
                    .getRemainingTime() <
                processes[currentProcess]
                    .getRemainingTime())
            {
                processes[currentProcess].changeState(
                    ProcessState::READY
                );
 
                processes[currentProcess]
                    .incrementPreemptionCount();
 
                ++preemptions;
 
                ++contextSwitches;
 
                currentProcess = selectedProcess;
 
                processes[currentProcess].changeState(
                    ProcessState::RUNNING
                );
 
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
            if (selectedProcess != -1)
            {
                currentProcess = selectedProcess;
 
                processes[currentProcess].changeState(
                    ProcessState::RUNNING
                );
 
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
         * Execute exactly one CPU time unit.
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
                time + process.getIODuration();
 
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
 
AlgorithmType SRTFScheduler::getAlgorithmType() const
{
    return AlgorithmType::SRTF;
}
 
