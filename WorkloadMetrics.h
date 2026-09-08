
#pragma once

#include <vector>
#include "WorkloadType.h"
#include "ArrivalPattern.h"
#include "VarianceLevel.h"

class WorkloadMetrics
{
private:
    int processCount;

    double averageBurstTime;
    double minimumBurstTime;
    double maximumBurstTime;
    double burstVariance;

    int priorityLevelCount;
    int highestPriorityProcessCount;

    int ioProcessCount;

    ArrivalPattern arrivalPattern;
    VarianceLevel varianceLevel;
    WorkloadType workloadType;

public:
    WorkloadMetrics();

    int getProcessCount() const;

    double getAverageBurstTime() const;
    double getMinimumBurstTime() const;
    double getMaximumBurstTime() const;
    double getBurstVariance() const;

    int getPriorityLevelCount() const;
    int getHighestPriorityProcessCount() const;

    int getIOProcessCount() const;

    ArrivalPattern getArrivalPattern() const;
    VarianceLevel getVarianceLevel() const;
    WorkloadType getWorkloadType() const;

    void setProcessCount(int value);

    void setAverageBurstTime(double value);
    void setMinimumBurstTime(double value);
    void setMaximumBurstTime(double value);
    void setBurstVariance(double value);

    void setPriorityLevelCount(int value);
    void setHighestPriorityProcessCount(int value);

    void setIOProcessCount(int value);

    void setArrivalPattern(ArrivalPattern value);
    void setVarianceLevel(VarianceLevel value);
    void setWorkloadType(WorkloadType value);
};
