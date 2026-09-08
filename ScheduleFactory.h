
#pragma once
 
#include <memory>
#include "Scheduler.h"
 
class SchedulerFactory
{
public:
    static std::unique_ptr<Scheduler> create(
        AlgorithmType algorithm
    );
};
