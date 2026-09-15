#include "RoundRobinScheduler.h"
#include "SimulationEvent.h"

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
    std::vector<ProcessModel> processes = inputProcesses;

    ExecutionResult result(
        AlgorithmType::ROUND_ROBIN
    );

    const std::size_t processCount = processes.size();

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

                changeProcessState(
                    processes[i],
                    ProcessState::READY,
                    time
                );

                notifyEvent(
                    SimulationEvent(
                        processes[i].getName(),
                        time,
                        SimulationEventType::ARRIVAL
                    )
                );

                readyQueue.push(i);
            }
        }

        /*
         * Complete CSV-defined I/O operations.
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
                        SimulationEventType::IO_COMPLETED
                    )
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

                changeProcessState(
                    processes[currentProcess],
                    ProcessState::RUNNING,
                    time
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

                const std::string& currentName =
                    processes[currentProcess]
                        .getName();

                /*
                 * Context switch occurs only when
                 * CPU changes from one process to another.
                 */
                if (!previousProcess.empty() &&
                    previousProcess != currentName)
                {
                    ++contextSwitches;

                    notifyEvent(
                        SimulationEvent(
                            currentName,
                            time,
                            SimulationEventType::CONTEXT_SWITCH
                        )
                    );
                }

                previousProcess = currentName;

                notifyEvent(
                    SimulationEvent(
                        currentName,
                        time,
                        SimulationEventType::CPU_ALLOCATED
                    )
                );
            }
            else
            {
                /*
                 * CPU idle.
                 */
                ++idleTime;

                notifyEvent(
                    SimulationEvent(
                        "CPU",
                        time,
                        SimulationEventType::CPU_IDLE
                    )
                );

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
                SimulationEventType::PROCESS_EXECUTION
            )
        );

        /*
         * Process completed.
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
                    SimulationEventType::PROCESS_COMPLETED
                )
            );

            ++completedCount;

            currentProcess = -1;
            quantumUsed = 0;

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

            notifyEvent(
                SimulationEvent(
                    process.getName(),
                    time,
                    SimulationEventType::IO_TRIGGER
                )
            );

            changeProcessState(
                process,
                ProcessState::WAITING,
                time
            );

            ioEndTime[currentProcess] =
                time +
                process.getIODuration();

            currentProcess = -1;
            quantumUsed = 0;

            continue;
        }

        /*
         * Round Robin quantum expired.
         */
        if (quantumUsed >= timeQuantum)
        {
            changeProcessState(
                process,
                ProcessState::READY,
                time
            );

            readyQueue.push(
                static_cast<std::size_t>(
                    currentProcess
                )
            );

            ++preemptions;

            notifyEvent(
                SimulationEvent(
                    process.getName(),
                    time,
                    SimulationEventType::PREEMPTION
                )
            );

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
