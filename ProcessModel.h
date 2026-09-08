
#pragma once

#include <string>
#include <vector>
#include "ProcessState.h"

class ProcessModel
{
private:
    // Basic process information
    std::string name;
    int arrivalTime;
    int burstTime;

    // Priority
    int originalPriority;
    int effectivePriority;

    // Execution information
    int remainingTime;
    int executedTime;

    // I/O information
    int ioTriggerTime;
    int ioDuration;
    bool ioCompleted;

    // Scheduling information
    int readyWaitingTime;
    int agingEvents;
    int preemptionCount;

    // Metrics
    int startTime;
    int completionTime;

    // State
    ProcessState currentState;
    std::vector<ProcessState> stateHistory;

public:
    ProcessModel(
        const std::string& name,
        int arrivalTime,
        int burstTime,
        int priority,
        int ioTriggerTime = -1,
        int ioDuration = 0
    );

    // Basic information
    const std::string& getName() const;
    int getArrivalTime() const;
    int getBurstTime() const;

    // Priority
    int getOriginalPriority() const;
    int getEffectivePriority() const;
    void improveEffectivePriority();

    // Execution
    int getRemainingTime() const;
    int getExecutedTime() const;
    void executeOneUnit();
    void setRemainingTime(int time);

    // I/O
    int getIOTriggerTime() const;
    int getIODuration() const;
    bool hasIO() const;
    bool isIOCompleted() const;
    void markIOCompleted();

    // Waiting / aging
    int getReadyWaitingTime() const;
    int getAgingEvents() const;
    void incrementReadyWaitingTime();
    void resetReadyWaitingTime();
    void incrementAgingEvents();

    // Scheduling
    int getPreemptionCount() const;
    void incrementPreemptionCount();

    // Metrics
    int getStartTime() const;
    int getCompletionTime() const;
    void setStartTime(int time);
    void setCompletionTime(int time);

    // State
    ProcessState getState() const;
    void changeState(ProcessState newState);
    const std::vector<ProcessState>& getStateHistory() const;

    // Status
    bool isCompleted() const;
};
