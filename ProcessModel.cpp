#include "ProcessModel.h"
#include "StateTransitionValidator.h"

ProcessModel::ProcessModel(
    const std::string& name,
    int arrivalTime,
    int burstTime,
    int priority,
    int ioTriggerTime,
    int ioDuration
)
    : name(name),
      arrivalTime(arrivalTime),
      burstTime(burstTime),
      originalPriority(priority),
      effectivePriority(priority),
      remainingTime(burstTime),
      executedTime(0),
      ioTriggerTime(ioTriggerTime),
      ioDuration(ioDuration),
      ioCompleted(false),
      readyWaitingTime(0),
      agingEvents(0),
      preemptionCount(0),
      startTime(-1),
      completionTime(-1),
      currentState(ProcessState::NEW)
{
    stateHistory.push_back(ProcessState::NEW);
}


// Basic information

const std::string& ProcessModel::getName() const
{
    return name;
}

int ProcessModel::getArrivalTime() const
{
    return arrivalTime;
}

int ProcessModel::getBurstTime() const
{
    return burstTime;
}


// Priority

int ProcessModel::getOriginalPriority() const
{
    return originalPriority;
}

int ProcessModel::getEffectivePriority() const
{
    return effectivePriority;
}

void ProcessModel::improveEffectivePriority()
{
    if (effectivePriority > 1)
    {
        --effectivePriority;
    }
}


// Execution

int ProcessModel::getRemainingTime() const
{
    return remainingTime;
}

int ProcessModel::getExecutedTime() const
{
    return executedTime;
}

void ProcessModel::executeOneUnit()
{
    if (remainingTime > 0)
    {
        --remainingTime;
        ++executedTime;
    }
}

void ProcessModel::setRemainingTime(int time)
{
    remainingTime = time;
}


// I/O

int ProcessModel::getIOTriggerTime() const
{
    return ioTriggerTime;
}

int ProcessModel::getIODuration() const
{
    return ioDuration;
}

bool ProcessModel::hasIO() const
{
    return ioTriggerTime >= 0 && ioDuration > 0;
}

bool ProcessModel::isIOCompleted() const
{
    return ioCompleted;
}

void ProcessModel::markIOCompleted()
{
    ioCompleted = true;
}


// Waiting / aging

int ProcessModel::getReadyWaitingTime() const
{
    return readyWaitingTime;
}

int ProcessModel::getAgingEvents() const
{
    return agingEvents;
}

void ProcessModel::incrementReadyWaitingTime()
{
    ++readyWaitingTime;
}

void ProcessModel::resetReadyWaitingTime()
{
    readyWaitingTime = 0;
}

void ProcessModel::incrementAgingEvents()
{
    ++agingEvents;
}


// Scheduling

int ProcessModel::getPreemptionCount() const
{
    return preemptionCount;
}

void ProcessModel::incrementPreemptionCount()
{
    ++preemptionCount;
}


// Metrics

int ProcessModel::getStartTime() const
{
    return startTime;
}

int ProcessModel::getCompletionTime() const
{
    return completionTime;
}

void ProcessModel::setStartTime(int time)
{
    startTime = time;
}

void ProcessModel::setCompletionTime(int time)
{
    completionTime = time;
}


// State

ProcessState ProcessModel::getState() const
{
    return currentState;
}

void ProcessModel::changeState(ProcessState newState)
{
    if (currentState == newState)
    {
        return;
    }

    if (!StateTransitionValidator::isValid(currentState, newState))
    {
        return;
    }

    currentState = newState;
    stateHistory.push_back(newState);
}

const std::vector<ProcessState>& ProcessModel::getStateHistory() const
{
    return stateHistory;
}


// Status

bool ProcessModel::isCompleted() const
{
    return currentState == ProcessState::COMPLETED;
}
