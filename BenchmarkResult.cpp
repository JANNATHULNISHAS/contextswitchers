#include "BenchmarkResult.h"
 
BenchmarkResult::BenchmarkResult(AlgorithmType algorithm)
    : algorithm(algorithm),
      averageWaitingTime(0.0),
      averageTurnaroundTime(0.0),
      averageResponseTime(0.0),
      throughput(0.0),
      cpuUtilization(0.0),
      averageContextSwitchTime(0.0),
      rankingScore(0.0),
      contextSwitches(0),
      preemptions(0),
      rank(0)
{
}
 
AlgorithmType BenchmarkResult::getAlgorithm() const
{
    return algorithm;
}
 
double BenchmarkResult::getAverageWaitingTime() const
{
    return averageWaitingTime;
}
 
double BenchmarkResult::getAverageTurnaroundTime() const
{
    return averageTurnaroundTime;
}
 
double BenchmarkResult::getAverageResponseTime() const
{
    return averageResponseTime;
}
 
double BenchmarkResult::getThroughput() const
{
    return throughput;
}
 
double BenchmarkResult::getCPUUtilization() const
{
    return cpuUtilization;
}

double BenchmarkResult::getAverageContextSwitchTime() const
{
    return averageContextSwitchTime;
}

double BenchmarkResult::getRankingScore() const
{
    return rankingScore;
}

int BenchmarkResult::getContextSwitches() const
{
    return contextSwitches;
}
 
int BenchmarkResult::getPreemptions() const
{
    return preemptions;
}
 
int BenchmarkResult::getRank() const
{
    return rank;
}
 
void BenchmarkResult::setAverageWaitingTime(double value)
{
    averageWaitingTime = value;
}
 
void BenchmarkResult::setAverageTurnaroundTime(double value)
{
    averageTurnaroundTime = value;
}
 
void BenchmarkResult::setAverageResponseTime(double value)
{
    averageResponseTime = value;
}
 
void BenchmarkResult::setThroughput(double value)
{
    throughput = value;
}
 
void BenchmarkResult::setCPUUtilization(double value)
{
    cpuUtilization = value;
}
 
void BenchmarkResult::setContextSwitches(int value)
{
    contextSwitches = value;
}
 
void BenchmarkResult::setPreemptions(int value)
{
    preemptions = value;
}
 
void BenchmarkResult::setRank(int value)
{
    rank = value;
}

void BenchmarkResult::setAverageContextSwitchTime(double value)
{
    averageContextSwitchTime = value;
}

void BenchmarkResult::setRankingScore(double value)
{
    rankingScore = value;
}
