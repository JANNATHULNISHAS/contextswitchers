#include "PreemptivePriorityScheduler.h"

#include <vector>
#include <string>

#include "SimulationEvent.h"

ExecutionResult PreemptivePriorityScheduler::schedule(
    const std::vector<ProcessModel>& inputProcesses)
{
    std::vector<ProcessModel> processes =
        inputProcesses;

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
     * Used for the 5-unit aging rule.
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
         * -------------------------------------------------
         * 1. PROCESS ARRIVALS
         * -------------------------------------------------
         */
        for (std::size_t i = 0;
             i < processCount;
             ++i)
        {
            if (!admitted[i] &&
                processes[i].getArrivalTime() <= time)
            {
                admitted[i] = true;
 
changeProcessState(
    processes[i],
    ProcessState::READY,
    time
);
 
readySince[i] = time;
 
                notifyEvent(
                    SimulationEvent(
                        processes[i].getName(),
                        time,
                        SimulationEventType::ARRIVAL,
                        "Process arrived and entered READY queue"
                    )
                );
            }
        }

        /*
         * -------------------------------------------------
         * 2. COMPLETE CSV-DEFINED I/O
         * -------------------------------------------------
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
                changeProcessState(
                    processes[i],
                    ProcessState::READY,
                    time
                );

                readySince[i] = time;

                ioEndTime[i] = -1;

                notifyEvent(
                    SimulationEvent(
                        processes[i].getName(),
                        time,
                        SimulationEventType::IO_COMPLETED,
                        "I/O completed; process returned to READY queue"
                    )
                );
            }
        }

        /*
         * -------------------------------------------------
         * 3. AGING
         * -------------------------------------------------
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
                        const int oldPriority =
                            processes[i]
                                .getEffectivePriority();

                        processes[i]
                            .improveEffectivePriority();

                        processes[i]
                            .incrementAgingEvents();

                        const int newPriority =
                            processes[i]
                                .getEffectivePriority();

                        if (oldPriority != newPriority)
                        {
                            notifyEvent(
                                SimulationEvent(
                                    processes[i].getName(),
                                    time,
                                    SimulationEventType::AGING,
                                    "Aging improved effective priority from "
                                    + std::to_string(oldPriority)
                                    + " to "
                                    + std::to_string(newPriority)
                                )
                            );
                        }
                    }
                }
            }
        }

        /*
         * -------------------------------------------------
         * 4. FIND HIGHEST-PRIORITY READY PROCESS
         * -------------------------------------------------
         *
         * Lower priority number = higher priority.
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
         * -------------------------------------------------
         * 5. PREEMPTION CHECK
         * -------------------------------------------------
         */
        if (currentProcess != -1)
        {
            if (selectedProcess != -1 &&
                processes[selectedProcess]
                    .getEffectivePriority() <
                processes[currentProcess]
                    .getEffectivePriority())
            {
                const std::string oldProcessName =
                    processes[currentProcess]
                        .getName();

                const std::string newProcessName =
                    processes[selectedProcess]
                        .getName();

                changeProcessState(
                    processes[currentProcess],
                    ProcessState::READY,
                    time
                );

                readySince[currentProcess] =
                    time;

                processes[currentProcess]
                    .incrementPreemptionCount();

                ++preemptions;
                ++contextSwitches;

                notifyEvent(
                    SimulationEvent(
                        oldProcessName,
                        time,
                        SimulationEventType::PREEMPTION,
                        "Preempted by " + newProcessName
                    )
                );

                notifyEvent(
                    SimulationEvent(
                        newProcessName,
                        time,
                        SimulationEventType::CONTEXT_SWITCH,
                        "CPU switched from "
                        + oldProcessName
                        + " to "
                        + newProcessName
                    )
                );

                currentProcess =
                    selectedProcess;

                changeProcessState(
                    processes[currentProcess],
                    ProcessState::RUNNING,
                    time
                );

                readySince[currentProcess] = -1;

                if (processes[currentProcess]
                        .getStartTime() == -1)
                {
                    processes[currentProcess]
                        .setStartTime(time);
                }

                notifyEvent(
                    SimulationEvent(
                        newProcessName,
                        time,
                        SimulationEventType::CPU_ALLOCATED,
                        "CPU allocated after preemption"
                    )
                );

                previousProcess =
                    newProcessName;
            }
        }
        else
        {
            /*
             * -------------------------------------------------
             * CPU IS FREE
             * -------------------------------------------------
             */
            if (selectedProcess != -1)
            {
                currentProcess =
                    selectedProcess;

                changeProcessState(
                    processes[currentProcess],
                    ProcessState::RUNNING,
                    time
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

                    notifyEvent(
                        SimulationEvent(
                            currentName,
                            time,
                            SimulationEventType::CONTEXT_SWITCH,
                            "CPU switched from "
                            + previousProcess
                            + " to "
                            + currentName
                        )
                    );
                }

                previousProcess =
                    currentName;

                notifyEvent(
                    SimulationEvent(
                        currentName,
                        time,
                        SimulationEventType::CPU_ALLOCATED,
                        "CPU allocated"
                    )
                );
            }
            else
            {
                /*
                 * -------------------------------------------------
                 * CPU IDLE
                 * -------------------------------------------------
                 */
                notifyEvent(
                    SimulationEvent(
                        "CPU",
                        time,
                        SimulationEventType::CPU_IDLE,
                        "No READY process; CPU idle"
                    )
                );

                ++idleTime;
                ++time;

                continue;
            }
        }

        /*
         * -------------------------------------------------
         * 6. EXECUTE ONE CPU TIME UNIT
         * -------------------------------------------------
         */
        ProcessModel& process =
            processes[currentProcess];

        const int sliceStart =
            time;

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

        notifyEvent(
            SimulationEvent(
                process.getName(),
                sliceStart,
                SimulationEventType::PROCESS_EXECUTION,
                process.getName() + " executed for 1 CPU time unit"
            )
        );

        /*
         * -------------------------------------------------
         * 7. PROCESS COMPLETED
         * -------------------------------------------------
         */
        if (process.getRemainingTime() == 0)
        {
            changeProcessState(
                process,
                ProcessState::COMPLETED,
                time
            );

            process.setCompletionTime(time);

            result.addCompletedProcess(
                process.getName()
            );

            notifyEvent(
                SimulationEvent(
                    process.getName(),
                    time,
                    SimulationEventType::PROCESS_COMPLETED,
                    "Process completed execution"
                )
            );

            ++completedCount;

            currentProcess = -1;

            continue;
        }

        /*
         * -------------------------------------------------
         * 8. CSV-DEFINED I/O TRIGGER
         * -------------------------------------------------
         */
        if (process.hasIO() &&
            !process.isIOCompleted() &&
            process.getExecutedTime() >=
                process.getIOTriggerTime())
        {
            process.markIOCompleted();

            changeProcessState(
                process,
                ProcessState::WAITING,
                time
            );

            ioEndTime[currentProcess] =
                time +
                process.getIODuration();

            notifyEvent(
                SimulationEvent(
                    process.getName(),
                    time,
                    SimulationEventType::IO_TRIGGER,
                    "I/O triggered; duration = "
                    + std::to_string(
                        process.getIODuration()
                    )
                    + " time units"
                )
            );

            currentProcess = -1;
        }
    }

    /*
     * -------------------------------------------------
     * FINAL RESULT STATISTICS
     * -------------------------------------------------
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
PreemptivePriorityScheduler::getAlgorithmType() const
{
    return AlgorithmType::PRIORITY_PREEMPTIVE;
}
