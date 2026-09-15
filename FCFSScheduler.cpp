#include "FCFSScheduler.h"
#include "SimulationEvent.h"

#include <queue>
#include <vector>
#include <string>

ExecutionResult FCFSScheduler::schedule(
    const std::vector<ProcessModel>& inputProcesses)
{
    std::vector<ProcessModel> processes = inputProcesses;

    ExecutionResult result(AlgorithmType::FCFS);

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

    /*
     * Stores the exact simulation time
     * at which I/O finishes.
     */
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

                readyQueue.push(i);

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
         * Complete I/O only at the exact I/O completion time.
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

                readyQueue.push(i);

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
         * FCFS is non-preemptive.
         *
         * Select another process only when
         * CPU currently has no running process.
         * --------------------------------------------------
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
                 * Context switch occurs only when
                 * switching from one process to
                 * another process.
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
                            "Context switch to " + currentName
                        )
                    );
                }

                previousProcess = currentName;
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

        ProcessModel& process =
            processes[currentProcess];

        const int sliceStart = time;

        /*
         * --------------------------------------------------
         * Execute one CPU time unit.
         * --------------------------------------------------
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
         * Notify dashboard/logger about CPU execution.
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
         * Completion has priority over I/O.
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
         * Enter WAITING only when the CSV-defined
         * I/O trigger is reached.
         *
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

            /*
             * Example:
             *
             * CPU execution ends at t = 5
             * I/O duration = 3
             * Process becomes READY at t = 8.
             */
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

    return result;
}

AlgorithmType FCFSScheduler::getAlgorithmType() const
{
    return AlgorithmType::FCFS;
}
