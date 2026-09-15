#include "SRTFScheduler.h"
#include "SimulationEvent.h"

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
         * --------------------------------------------------
         * Admit newly arrived processes.
         * NEW -> READY
         * --------------------------------------------------
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
         * --------------------------------------------------
         * Complete I/O operations.
         * WAITING -> READY
         * --------------------------------------------------
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

                notifyEvent(
                    SimulationEvent(
                        processes[i].getName(),
                        time,
                        SimulationEventType::IO_COMPLETED,
                        "I/O completed and process entered READY queue"
                    )
                );

                ioEndTime[i] = -1;
            }
        }

        /*
         * --------------------------------------------------
         * Find the READY process with the
         * shortest remaining time.
         * --------------------------------------------------
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
         * --------------------------------------------------
         * Decide whether current process should continue
         * or be preempted.
         * --------------------------------------------------
         */
        if (currentProcess != -1)
        {
            if (selectedProcess != -1 &&
                processes[selectedProcess]
                    .getRemainingTime() <
                processes[currentProcess]
                    .getRemainingTime())
            {
                /*
                 * RUNNING -> READY
                 */
                changeProcessState(
                    processes[currentProcess],
                    ProcessState::READY,
                    time
                );

                processes[currentProcess]
                    .incrementPreemptionCount();

                ++preemptions;

                ++contextSwitches;

                notifyEvent(
                    SimulationEvent(
                        processes[currentProcess].getName(),
                        time,
                        SimulationEventType::PREEMPTION,
                        "Process preempted by shorter remaining-time process"
                    )
                );

                /*
                 * Select new process.
                 */
                currentProcess =
                    selectedProcess;

                /*
                 * READY -> RUNNING
                 */
                changeProcessState(
                    processes[currentProcess],
                    ProcessState::RUNNING,
                    time
                );

                if (processes[currentProcess]
                        .getStartTime() == -1)
                {
                    processes[currentProcess]
                        .setStartTime(time);
                }

                notifyEvent(
                    SimulationEvent(
                        processes[currentProcess].getName(),
                        time,
                        SimulationEventType::CONTEXT_SWITCH,
                        "Context switch to "
                        + processes[currentProcess].getName()
                    )
                );

                previousProcess =
                    processes[currentProcess]
                        .getName();
            }
        }
        else
        {
            /*
             * --------------------------------------------------
             * CPU has no running process.
             * --------------------------------------------------
             */
            if (selectedProcess != -1)
            {
                currentProcess =
                    selectedProcess;

                /*
                 * READY -> RUNNING
                 */
                changeProcessState(
                    processes[currentProcess],
                    ProcessState::RUNNING,
                    time
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

                    notifyEvent(
                        SimulationEvent(
                            currentName,
                            time,
                            SimulationEventType::CONTEXT_SWITCH,
                            "Context switch to " + currentName
                        )
                    );
                }

                previousProcess =
                    currentName;
            }
            else
            {
                /*
                 * --------------------------------------------------
                 * CPU IDLE
                 * --------------------------------------------------
                 */
                notifyEvent(
                    SimulationEvent(
                        "",
                        time,
                        SimulationEventType::CPU_IDLE,
                        "CPU is idle"
                    )
                );

                ++idleTime;
                ++time;

                continue;
            }
        }

        /*
         * --------------------------------------------------
         * Execute exactly one CPU time unit.
         * --------------------------------------------------
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
         * Notify dashboard/logger.
         */
        notifyEvent(
            SimulationEvent(
                process.getName(),
                sliceStart,
                SimulationEventType::PROCESS_EXECUTION,
                process.getName() + " executed"
            )
        );

        /*
         * --------------------------------------------------
         * Process completed.
         * RUNNING -> COMPLETED
         * --------------------------------------------------
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

            ++completedCount;

            currentProcess = -1;

            continue;
        }

        /*
         * --------------------------------------------------
         * CSV-defined I/O trigger.
         * RUNNING -> WAITING
         * --------------------------------------------------
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

            notifyEvent(
                SimulationEvent(
                    process.getName(),
                    time,
                    SimulationEventType::IO_TRIGGER,
                    "Process entered WAITING for I/O"
                )
            );

            ioEndTime[currentProcess] =
                time + process.getIODuration();

            currentProcess = -1;
        }
    }

    /*
     * --------------------------------------------------
     * Store final execution metrics.
     * --------------------------------------------------
     */
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
