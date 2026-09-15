#include "LiveDashboard.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

#include "SimulationEvent.h"

LiveDashboard::LiveDashboard(
    const std::string& applicationName,
    AlgorithmType algorithm,
    const std::vector<ProcessModel>& processList,
    int refreshDelayMs
)
    : applicationName(applicationName),
      algorithm(algorithm),
      currentTime(0),
      cpuBusyTime(0),
      cpuIdleTime(0),
      contextSwitches(0),
      preemptions(0),
      refreshDelayMs(refreshDelayMs)
{
    for (std::size_t i = 0; i < processList.size(); ++i)
    {
        DashboardProcess process;

        process.name = processList[i].getName();
        process.arrivalTime = processList[i].getArrivalTime();
        process.burstTime = processList[i].getBurstTime();
        process.remainingTime = processList[i].getRemainingTime();

        process.originalPriority =
            processList[i].getOriginalPriority();

        process.effectivePriority =
            processList[i].getEffectivePriority();

        process.state =
            processList[i].getState();

        process.preemptionCount = 0;

        processes[process.name] = process;
    }
}


LiveDashboard::~LiveDashboard()
{
}


/*
 * ------------------------------------------------------------
 * EVENT HANDLING
 * ------------------------------------------------------------
 */
void LiveDashboard::onEvent(const SimulationEvent& event)
{
    const std::string& processName =
        event.getProcessName();

    if (!processName.empty() &&
        processes.find(processName) != processes.end())
    {
        DashboardProcess& process =
            processes[processName];

        switch (event.getEventType())
        {
            case SimulationEventType::STATE_CHANGE:
            {
                process.state =
                    event.getTransition().to;
                break;
            }

            case SimulationEventType::PROCESS_EXECUTION:
            {
                if (process.remainingTime > 0)
                {
                    --process.remainingTime;
                }

                ++cpuBusyTime;

                currentTime =
                    event.getTimestamp() + 1;

                break;
            }

            case SimulationEventType::PREEMPTION:
            {
                ++preemptions;
                ++process.preemptionCount;
                break;
            }

            case SimulationEventType::AGING:
            {
                if (process.effectivePriority > 1)
                {
                    --process.effectivePriority;
                }

                break;
            }

            case SimulationEventType::PROCESS_COMPLETED:
            {
                process.state =
                    ProcessState::COMPLETED;

                process.remainingTime = 0;

                break;
            }

            default:
                break;
        }
    }

    if (event.getEventType() ==
        SimulationEventType::CONTEXT_SWITCH)
    {
        ++contextSwitches;
    }
    else if (event.getEventType() ==
             SimulationEventType::CPU_IDLE)
    {
        ++cpuIdleTime;

        currentTime =
            event.getTimestamp() + 1;
    }

    /*
     * Refresh only when simulation time advances.
     */
    if (event.getEventType() ==
            SimulationEventType::PROCESS_EXECUTION ||
        event.getEventType() ==
            SimulationEventType::CPU_IDLE)
    {
        refresh(event);

        if (refreshDelayMs > 0)
        {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(
                    refreshDelayMs
                )
            );
        }
    }
}


/*
 * ------------------------------------------------------------
 * CLEAR SCREEN
 * ------------------------------------------------------------
 */
void LiveDashboard::clearScreen() const
{
#ifdef _WIN32

    HANDLE console =
        GetStdHandle(STD_OUTPUT_HANDLE);

    if (console == INVALID_HANDLE_VALUE ||
        console == NULL)
    {
        return;
    }

    CONSOLE_SCREEN_BUFFER_INFO screenInfo;

    if (!GetConsoleScreenBufferInfo(
            console,
            &screenInfo))
    {
        return;
    }

    COORD topLeft;
    topLeft.X = 0;
    topLeft.Y = 0;

    DWORD consoleSize =
        static_cast<DWORD>(
            screenInfo.dwSize.X *
            screenInfo.dwSize.Y
        );

    DWORD charactersWritten;

    /*
     * Erase the complete console buffer.
     */
    FillConsoleOutputCharacterA(
        console,
        ' ',
        consoleSize,
        topLeft,
        &charactersWritten
    );

    /*
     * Restore the console attributes.
     */
    FillConsoleOutputAttribute(
        console,
        screenInfo.wAttributes,
        consoleSize,
        topLeft,
        &charactersWritten
    );

    /*
     * Move cursor to the top-left.
     */
    SetConsoleCursorPosition(
        console,
        topLeft
    );

#else

    /*
     * ANSI terminal fallback.
     */
    std::cout << "\033[2J\033[H";

#endif

    std::cout.flush();
}


/*
 * ------------------------------------------------------------
 * REFRESH
 * ------------------------------------------------------------
 */
void LiveDashboard::refresh(
    const SimulationEvent& event
)
{
    clearScreen();

    displayHeader();
    displayCPUStatus();
    displayProcessTable();
    displayReadyQueue();

    std::cout << std::endl;

    displayEvent(event);

    std::cout.flush();
}


/*
 * ------------------------------------------------------------
 * HEADER
 * ------------------------------------------------------------
 */
void LiveDashboard::displayHeader() const
{
    std::cout
        << "============================================================"
        << std::endl;

    std::cout
        << "                 LIVE CPU SCHEDULER DASHBOARD"
        << std::endl;

    std::cout
        << "============================================================"
        << std::endl;

    std::cout
        << "Application : "
        << applicationName
        << std::endl;

    std::cout
        << "Algorithm   : "
        << algorithmToString(algorithm)
        << std::endl;

    std::cout << std::endl;
}


/*
 * ------------------------------------------------------------
 * CPU STATUS
 * ------------------------------------------------------------
 */
void LiveDashboard::displayCPUStatus() const
{
    std::cout
        << "---------------------- CPU STATUS --------------------------"
        << std::endl;

    std::cout
        << "Simulation Time : "
        << currentTime
        << std::endl;

    std::cout
        << "CPU Busy Time   : "
        << cpuBusyTime
        << std::endl;

    std::cout
        << "CPU Idle Time   : "
        << cpuIdleTime
        << std::endl;

    std::cout
        << "Context Switches: "
        << contextSwitches
        << std::endl;

    std::cout
        << "Preemptions     : "
        << preemptions
        << std::endl;

    std::cout << std::endl;
}


/*
 * ------------------------------------------------------------
 * PROCESS TABLE
 * ------------------------------------------------------------
 */
void LiveDashboard::displayProcessTable() const
{
    std::cout
        << "--------------------- PROCESS STATUS -----------------------"
        << std::endl;

    std::cout
        << std::left
        << std::setw(23) << "Process"
        << std::setw(5)  << "AT"
        << std::setw(5)  << "BT"
        << std::setw(6)  << "Rem"
        << std::setw(6)  << "Pri"
        << std::setw(8)  << "EffPri"
        << "State"
        << std::endl;

    std::cout
        << "------------------------------------------------------------"
        << std::endl;

    for (std::map<std::string,
                  DashboardProcess>::const_iterator it =
             processes.begin();
         it != processes.end();
         ++it)
    {
        const DashboardProcess& process =
            it->second;

        std::cout
            << std::left
            << std::setw(23) << process.name
            << std::setw(5)  << process.arrivalTime
            << std::setw(5)  << process.burstTime
            << std::setw(6)  << process.remainingTime
            << std::setw(6)  << process.originalPriority
            << std::setw(8)  << process.effectivePriority
            << stateToString(process.state)
            << std::endl;
    }

    std::cout << std::endl;
}


/*
 * ------------------------------------------------------------
 * READY QUEUE
 * ------------------------------------------------------------
 */
void LiveDashboard::displayReadyQueue() const
{
    std::vector<DashboardProcess> readyProcesses;

    for (std::map<std::string,
                  DashboardProcess>::const_iterator it =
             processes.begin();
         it != processes.end();
         ++it)
    {
        if (it->second.state ==
            ProcessState::READY)
        {
            readyProcesses.push_back(
                it->second
            );
        }
    }

    std::sort(
        readyProcesses.begin(),
        readyProcesses.end(),
        [](const DashboardProcess& a,
           const DashboardProcess& b)
        {
            if (a.effectivePriority !=
                b.effectivePriority)
            {
                return a.effectivePriority <
                       b.effectivePriority;
            }

            if (a.arrivalTime !=
                b.arrivalTime)
            {
                return a.arrivalTime <
                       b.arrivalTime;
            }

            return a.name < b.name;
        }
    );

    std::cout
        << "----------------------- READY QUEUE -----------------------"
        << std::endl;

    if (readyProcesses.empty())
    {
        std::cout
            << "[ empty ]"
            << std::endl;

        return;
    }

    for (std::size_t i = 0;
         i < readyProcesses.size();
         ++i)
    {
        std::cout
            << (i + 1)
            << ". "
            << readyProcesses[i].name
            << " [Priority "
            << readyProcesses[i].effectivePriority
            << "]"
            << std::endl;
    }
}


/*
 * ------------------------------------------------------------
 * LATEST EVENT
 * ------------------------------------------------------------
 */
void LiveDashboard::displayEvent(
    const SimulationEvent& event
) const
{
    std::cout
        << "---------------------- LATEST EVENT -----------------------"
        << std::endl;

    std::cout
        << "["
        << event.getTimestamp()
        << "] ";

    if (event.getEventType() ==
        SimulationEventType::STATE_CHANGE)
    {
        std::cout
            << "STATE CHANGE : "
            << event.getProcessName()
            << " ("
            << stateToString(
                event.getTransition().from
            )
            << " -> "
            << stateToString(
                event.getTransition().to
            )
            << ")";
    }
    else
    {
        if (!event.getProcessName().empty())
        {
            std::cout
                << event.getProcessName()
                << " : ";
        }

        std::cout
            << event.getDetails();
    }

    std::cout << std::endl;
}


/*
 * ------------------------------------------------------------
 * STATE TO STRING
 * ------------------------------------------------------------
 */
const char* LiveDashboard::stateToString(
    ProcessState state
) const
{
    switch (state)
    {
        case ProcessState::NEW:
            return "NEW";

        case ProcessState::READY:
            return "READY";

        case ProcessState::RUNNING:
            return "RUNNING";

        case ProcessState::WAITING:
            return "WAITING";

        case ProcessState::COMPLETED:
            return "COMPLETED";

        default:
            return "UNKNOWN";
    }
}


/*
 * ------------------------------------------------------------
 * ALGORITHM TO STRING
 * ------------------------------------------------------------
 */
const char* LiveDashboard::algorithmToString(
    AlgorithmType algorithm
) const
{
    switch (algorithm)
    {
        case AlgorithmType::FCFS:
            return "FCFS";

        case AlgorithmType::SJF:
            return "SJF";

        case AlgorithmType::SRTF:
            return "SRTF";

        case AlgorithmType::PRIORITY_NON_PREEMPTIVE:
            return "Priority Non-Preemptive";

        case AlgorithmType::PRIORITY_PREEMPTIVE:
            return "Priority Preemptive";

        case AlgorithmType::ROUND_ROBIN:
            return "Round Robin";

        default:
            return "Unknown";
    }
}
