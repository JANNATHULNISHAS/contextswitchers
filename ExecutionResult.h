
#pragma once

#include <string>
#include <vector>
#include "AlgorithmType.h"
#include "ExecutionSlice.h"

class ExecutionResult
{
private:
    AlgorithmType algorithm;
    std::vector<std::string> completedProcesses;
    std::vector<ExecutionSlice> timeline;

    int totalExecutionTime;
    int contextSwitches;
    int preemptions;

    int cpuBusyTime;
    int cpuIdleTime;

    double averageWaitingTime;
    double averageTurnaroundTime;
    double averageResponseTime;
    double throughput;
    double cpuUtilization;
    double averageContextSwitchTime;

public:
    ExecutionResult(AlgorithmType algorithm);

    AlgorithmType getAlgorithm() const;

    const std::vector<std::string>& getCompletedProcesses() const;
    const std::vector<ExecutionSlice>& getTimeline() const;

    int getTotalExecutionTime() const;
    int getContextSwitches() const;
    int getPreemptions() const;

    int getCPUBusyTime() const;
    int getCPUIdleTime() const;

    double getAverageWaitingTime() const;
    double getAverageTurnaroundTime() const;
    double getAverageResponseTime() const;
    double getThroughput() const;
    double getCPUUtilization() const;
    double getAverageContextSwitchTime() const;
    
    void addCompletedProcess(const std::string& processName);
    void addExecutionSlice(const ExecutionSlice& slice);

    void setTotalExecutionTime(int time);
    void setContextSwitches(int count);
    void setPreemptions(int count);

    void setCPUBusyTime(int time);
    void setCPUIdleTime(int time);

    void setAverageWaitingTime(double value);
    void setAverageTurnaroundTime(double value);
    void setAverageResponseTime(double value);
    void setThroughput(double value);
    void setCPUUtilization(double value);
    void setAverageContextSwitchTime(double value);
};
