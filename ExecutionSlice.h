
#pragma once

#include <string>

class ExecutionSlice
{
private:
    std::string processName;
    int startTime;
    int endTime;

public:
    ExecutionSlice(
        const std::string& processName,
        int startTime,
        int endTime
    );

    const std::string& getProcessName() const;
    int getStartTime() const;
    int getEndTime() const;
    int getDuration() const;
};
