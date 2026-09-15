#include "ExecutionLogger.h"
#include "SimulationEvent.h"

ExecutionLogger::ExecutionLogger()
{
}

ExecutionLogger::~ExecutionLogger()
{
    stop();
}

bool ExecutionLogger::start(
    const std::string& filePath)
{
    stop();
    stateTransitions.clear();

    logFile.open(
        filePath.c_str(),
        std::ios::out |
        std::ios::trunc
    );

    if (!logFile.is_open())
    {
        return false;
    }

    logFile
        << "================================================="
        << std::endl;

    logFile
        << "EXECUTION LOG REPORT"
        << std::endl;

    logFile
        << "================================================="
        << std::endl;

    logFile << std::endl;

    logFile.flush();

    return true;
}

void ExecutionLogger::stop()
{
    if (logFile.is_open())
    {
        logFile
            << std::endl
            << "================================================="
            << std::endl;

        logFile
            << "END OF LOG"
            << std::endl;

        logFile
            << "================================================="
            << std::endl;

        logFile.flush();

        logFile.close();
    }
}

void ExecutionLogger::onEvent(
    const SimulationEvent& event)
{
    if (event.getEventType() ==
        SimulationEventType::STATE_CHANGE)
    {
        stateTransitions.push_back(event);
    }
 
    if (!logFile.is_open())
    {
        return;
    }

    logFile
        << "["
        << event.getTimestamp()
        << "] ";

    logFile
        << eventTypeToString(
               event.getEventType()
           );

    logFile
        << " : ";

    if (!event.getDetails().empty())
    {
        logFile
            << event.getDetails();
    }
    else
    {
        logFile
            << event.getProcessName();
    }

    if (event.getEventType() ==
        SimulationEventType::STATE_CHANGE)
    {
        const StateTransition& transition =
            event.getTransition();

        logFile
            << " ("
            << processStateToString(
                   transition.from
               )
            << " -> "
            << processStateToString(
                   transition.to
               )
            << ")";
    }

    logFile << std::endl;

    /*
     * Important:
     * flush immediately so the file is updated
     * while the simulation is running.
     */
    logFile.flush();
}

const std::vector<SimulationEvent>&
ExecutionLogger::getStateTransitions() const
{
    return stateTransitions;
}

const char*
ExecutionLogger::eventTypeToString(
    SimulationEventType eventType) const
{
    switch (eventType)
    {
        case SimulationEventType::ARRIVAL:
            return "ARRIVAL";

        case SimulationEventType::STATE_CHANGE:
            return "STATE CHANGE";

        case SimulationEventType::CPU_ALLOCATED:
            return "CPU ALLOCATED";

        case SimulationEventType::PROCESS_EXECUTION:
            return "PROCESS EXECUTION";

        case SimulationEventType::IO_TRIGGER:
            return "I/O TRIGGER";

        case SimulationEventType::IO_COMPLETED:
            return "I/O COMPLETED";

        case SimulationEventType::CONTEXT_SWITCH:
            return "CONTEXT SWITCH";

        case SimulationEventType::PREEMPTION:
            return "PREEMPTION";

        case SimulationEventType::AGING:
            return "AGING";

        case SimulationEventType::PROCESS_COMPLETED:
            return "PROCESS COMPLETED";

        case SimulationEventType::CPU_IDLE:
            return "CPU IDLE";
    }

    return "UNKNOWN";
}

const char*
ExecutionLogger::processStateToString(
    ProcessState state) const
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
    }

    return "UNKNOWN";
}
