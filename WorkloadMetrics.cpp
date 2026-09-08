#include "WorkloadMetrics.h"

WorkloadMetrics::WorkloadMetrics()
    : processCount(0),
      averageBurstTime(0.0),
      minimumBurstTime(0.0),
      maximumBurstTime(0.0),
      burstVariance(0.0),
      priorityLevelCount(0),
      highestPriorityProcessCount(0),
      ioProcessCount(0),
      arrivalPattern(ArrivalPattern::MIXED),
      varianceLevel(VarianceLevel::LOW),
      workloadType(WorkloadType::MIXED)
{
}

int WorkloadMetrics::getProcessCount() const
{
    return processCount;
}

double WorkloadMetrics::getAverageBurstTime() const
{
    return averageBurstTime;
}

double WorkloadMetrics::getMinimumBurstTime() const
{
    return minimumBurstTime;
}

double WorkloadMetrics::getMaximumBurstTime() const
{
    return maximumBurstTime;
}

double WorkloadMetrics::getBurstVariance() const
{
    return burstVariance;
}

int WorkloadMetrics::getPriorityLevelCount() const
{
    return priorityLevelCount;
}

int WorkloadMetrics::getHighestPriorityProcessCount() const
{
    return highestPriorityProcessCount;
}

int WorkloadMetrics::getIOProcessCount() const
{
    return ioProcessCount;
}

ArrivalPattern WorkloadMetrics::getArrivalPattern() const
{
    return arrivalPattern;
}

VarianceLevel WorkloadMetrics::getVarianceLevel() const
{
    return varianceLevel;
}

WorkloadType WorkloadMetrics::getWorkloadType() const
{
    return workloadType;
}

void WorkloadMetrics::setProcessCount(int value)
{
    processCount = value;
}

void WorkloadMetrics::setAverageBurstTime(double value)
{
    averageBurstTime = value;
}

void WorkloadMetrics::setMinimumBurstTime(double value)
{
    minimumBurstTime = value;
}

void WorkloadMetrics::setMaximumBurstTime(double value)
{
    maximumBurstTime = value;
}

void WorkloadMetrics::setBurstVariance(double value)
{
    burstVariance = value;
}

void WorkloadMetrics::setPriorityLevelCount(int value)
{
    priorityLevelCount = value;
}

void WorkloadMetrics::setHighestPriorityProcessCount(int value)
{
    highestPriorityProcessCount = value;
}

void WorkloadMetrics::setIOProcessCount(int value)
{
    ioProcessCount = value;
}

void WorkloadMetrics::setArrivalPattern(ArrivalPattern value)
{
    arrivalPattern = value;
}

void WorkloadMetrics::setVarianceLevel(VarianceLevel value)
{
    varianceLevel = value;
}

void WorkloadMetrics::setWorkloadType(WorkloadType value)
{
    workloadType = value;
}
