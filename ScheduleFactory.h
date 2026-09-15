#pragma once

#include <string>

#include "StateTransition.h"

enum class SimulationEventType
{
    ARRIVAL,
    STATE_CHANGE,
    CPU_ALLOCATED,
    PROCESS_EXECUTION,
    IO_TRIGGER,
    IO_COMPLETED,
    CONTEXT_SWITCH,
    PREEMPTION,
    AGING,
    PROCESS_COMPLETED,
    CPU_IDLE
};

class SimulationEvent
{
private:
    std::string processName;
    int timestamp;

    SimulationEventType eventType;

    StateTransition transition;

    std::string details;

public:
    SimulationEvent(
        const std::string& processName,
        int timestamp,
        SimulationEventType eventType,
        const std::string& details = ""
    );

    SimulationEvent(
        const std::string& processName,
        int timestamp,
        ProcessState from,
        ProcessState to
    );

    const std::string& getProcessName() const;

    int getTimestamp() const;

    SimulationEventType getEventType() const;

    const StateTransition& getTransition() const;

    const std::string& getDetails() const;
};
