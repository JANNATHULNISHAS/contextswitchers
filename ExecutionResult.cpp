#include "ExecutionResult.h"

ExecutionResult::ExecutionResult(AlgorithmType algorithm)
    : algorithm(algorithm),
      totalExecutionTime(0),
      contextSwitches(0),
      preemptions(0),
      cpuBusyTime(0),
      cpuIdleTime(0),
      averageWaitingTime(0.0),
      averageTurnaroundTime(0.0),
      averageResponseTime(0.0),
      throughput(0.0),
      cpuUtilization(0.0),
      averageContextSwitchTime(0.0)
{
}


AlgorithmType ExecutionResult::getAlgorithm() const
{
    return algorithm;
}

const std::vector<std::string>&
ExecutionResult::getCompletedProcesses() const
{
    return completedProcesses;
}

const std::vector<ExecutionSlice>&
ExecutionResult::getTimeline() const
{
    return timeline;
}

int ExecutionResult::getTotalExecutionTime() const
{
    return totalExecutionTime;
}

int ExecutionResult::getContextSwitches() const
{
    return contextSwitches;
}

int ExecutionResult::getPreemptions() const
{
    return preemptions;
}

int ExecutionResult::getCPUBusyTime() const
{
    return cpuBusyTime;
}

int ExecutionResult::getCPUIdleTime() const
{
    return cpuIdleTime;
}

double ExecutionResult::getAverageWaitingTime() const
{
    return averageWaitingTime;
}

double ExecutionResult::getAverageTurnaroundTime() const
{
    return averageTurnaroundTime;
}

double ExecutionResult::getAverageResponseTime() const
{
    return averageResponseTime;
}

double ExecutionResult::getThroughput() const
{
    return throughput;
}

double ExecutionResult::getCPUUtilization() const
{
    return cpuUtilization;
}

double ExecutionResult::getAverageContextSwitchTime() const
{
    return averageContextSwitchTime;
}

void ExecutionResult::addCompletedProcess(
    const std::string& processName)
{
    completedProcesses.push_back(processName);
}

void ExecutionResult::addExecutionSlice(
    const ExecutionSlice& slice)
{
    timeline.push_back(slice);
}

void ExecutionResult::setTotalExecutionTime(int time)
{
    totalExecutionTime = time;
}

void ExecutionResult::setContextSwitches(int count)
{
    contextSwitches = count;
}

void ExecutionResult::setPreemptions(int count)
{
    preemptions = count;
}

void ExecutionResult::setCPUBusyTime(int time)
{
    cpuBusyTime = time;
}

void ExecutionResult::setCPUIdleTime(int time)
{
    cpuIdleTime = time;
}

void ExecutionResult::setAverageWaitingTime(double value)
{
    averageWaitingTime = value;
}

void ExecutionResult::setAverageTurnaroundTime(double value)
{
    averageTurnaroundTime = value;
}

void ExecutionResult::setAverageResponseTime(double value)
{
    averageResponseTime = value;
}

void ExecutionResult::setThroughput(double value)
{
    throughput = value;
}

void ExecutionResult::setCPUUtilization(double value)
{
    cpuUtilization = value;
}

void ExecutionResult::setAverageContextSwitchTime(double value)
{
    averageContextSwitchTime = value;
}
