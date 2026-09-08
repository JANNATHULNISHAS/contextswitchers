
#pragma once
 
#include "AlgorithmType.h"
 
class BenchmarkResult
{
private:
    AlgorithmType algorithm;
 
    double averageWaitingTime;
    double averageTurnaroundTime;
    double averageResponseTime;
 
    double throughput;
    double cpuUtilization;
 
    double averageContextSwitchTime;
    double rankingScore;
    
    int contextSwitches;
    int preemptions;
 
    int rank;
 
public:
    explicit BenchmarkResult(AlgorithmType algorithm);
 
    AlgorithmType getAlgorithm() const;
 
    double getAverageWaitingTime() const;
    double getAverageTurnaroundTime() const;
    double getAverageResponseTime() const;
 
    double getThroughput() const;
    double getCPUUtilization() const;
 
    double getAverageContextSwitchTime() const;
    double getRankingScore() const;

    int getContextSwitches() const;
    int getPreemptions() const;
 
    int getRank() const;
 
    void setAverageWaitingTime(double value);
    void setAverageTurnaroundTime(double value);
    void setAverageResponseTime(double value);
 
    void setThroughput(double value);
    void setCPUUtilization(double value);
 
    void setAverageContextSwitchTime(double value);
    void setRankingScore(double value);
    
    void setContextSwitches(int value);
    void setPreemptions(int value);
 
    void setRank(int value);
};
