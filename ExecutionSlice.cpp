#include "ExecutionSlice.h"

ExecutionSlice::ExecutionSlice(
    const std::string& processName,
    int startTime,
    int endTime
)
    : processName(processName),
      startTime(startTime),
      endTime(endTime)
{
}

const std::string& ExecutionSlice::getProcessName() const
{
    return processName;
}

int ExecutionSlice::getStartTime() const
{
    return startTime;
}

int ExecutionSlice::getEndTime() const
{
    return endTime;
}

int ExecutionSlice::getDuration() const
{
    return endTime - startTime;
}
