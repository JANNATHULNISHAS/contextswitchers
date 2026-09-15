#include "SJFScheduler.h"
#include "SimulationEvent.h"

#include <algorithm>
#include <vector>
#include <string>

ExecutionResult SJFScheduler::schedule(
    const std::vector<ProcessModel>& inputProcesses)
{
    std::vector<ProcessModel> processes = inputProcesses;

    ExecutionResult result(AlgorithmType::SJF);

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

                changeProcessState(
                    processes[i],
                    ProcessState::READY,
                    time
                );
            }
        }

        /*
         * Move processes from WAITING to READY
         * when their CSV-defined I/O has finished.
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

                ioEndTime[i] = -1;
            }
        }

        /*
         * SJF is non-preemptive.
         *
         * Select the READY process with the
         * smallest original burst time.
         *
         * Tie-breaking:
         * 1. Earlier arrival time
         * 2. Original vector order
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

                if (processes[i].getBurstTime() <
                    processes[selectedProcess]
                        .getBurstTime())
                {
                    selectedProcess =
                        static_cast<int>(i);
                }
                else if (
                    processes[i].getBurstTime() ==
                    processes[selectedProcess]
                        .getBurstTime() &&
                    processes[i].getArrivalTime() <
                    processes[selectedProcess]
                        .getArrivalTime())
                {
                    selectedProcess =
                        static_cast<int>(i);
                }
            }

            /*
             * A READY process was found.
             */
            if (selectedProcess != -1)
            {
                currentProcess = selectedProcess;

                ProcessModel& process =
                    processes[currentProcess];

                changeProcessState(
                    process,
                    ProcessState::RUNNING,
                    time
                );

                if (process.getStartTime() == -1)
                {
                    process.setStartTime(time);
                }

                const std::string& currentName =
                    process.getName();

                /*
                 * Count an actual process-to-process
                 * context switch.
                 */
                if (!previousProcess.empty() &&
                    previousProcess != currentName)
                {
                    ++contextSwitches;

                    notifyEvent(
                        SimulationEvent(
                            currentName,
                            time,
                            SimulationEventType::CONTEXT_SWITCH,
                            previousProcess +
                            " -> " +
                            currentName
                        )
                    );
                }

                previousProcess = currentName;
            }
            /*
             * No READY process exists.
             * CPU becomes idle for one time unit.
             */
            else
            {
                notifyEvent(
                    SimulationEvent(
                        "",
                        time,
                        SimulationEventType::CPU_IDLE,
                        "CPU idle"
                    )
                );

                ++idleTime;
                ++time;

                continue;
            }
        }

        ProcessModel& process =
            processes[currentProcess];

        const int sliceStart = time;

        /*
         * Execute exactly one CPU time unit.
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
         * Notify observers so that:
         * - LiveDashboard updates
         * - ExecutionLogger records execution
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
         * Check completion first.
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
         * Check CSV-defined I/O trigger.
         *
         * I/O is triggered only when the CSV
         * specifies an I/O trigger time.
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
                time + process.getIODuration();

            notifyEvent(
                SimulationEvent(
                    process.getName(),
                    time,
                    SimulationEventType::IO_TRIGGER,
                    "I/O triggered"
                )
            );

            currentProcess = -1;
        }
    }

    result.setTotalExecutionTime(time);
    result.setCPUBusyTime(busyTime);
    result.setCPUIdleTime(idleTime);
    result.setContextSwitches(contextSwitches);

    return result;
}

AlgorithmType SJFScheduler::getAlgorithmType() const
{
    return AlgorithmType::SJF;
}
