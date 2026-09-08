#include "SchedulerFactory.h"
 
#include "FCFSScheduler.h"
#include "SJFScheduler.h"
#include "SRTFScheduler.h"
#include "PriorityScheduler.h"
#include "PreemptivePriorityScheduler.h"
#include "RoundRobinScheduler.h"
 
std::unique_ptr<Scheduler> SchedulerFactory::create(
    AlgorithmType algorithm)
{
    switch (algorithm)
    {
        case AlgorithmType::FCFS:
            return std::unique_ptr<Scheduler>(
                new FCFSScheduler()
            );
 
        case AlgorithmType::SJF:
            return std::unique_ptr<Scheduler>(
                new SJFScheduler()
            );
 
        case AlgorithmType::SRTF:
            return std::unique_ptr<Scheduler>(
                new SRTFScheduler()
            );
 
        case AlgorithmType::PRIORITY_NON_PREEMPTIVE:
            return std::unique_ptr<Scheduler>(
                new PriorityScheduler()
            );
 
        case AlgorithmType::PRIORITY_PREEMPTIVE:
            return std::unique_ptr<Scheduler>(
                new PreemptivePriorityScheduler()
            );
 
        case AlgorithmType::ROUND_ROBIN:
            return std::unique_ptr<Scheduler>(
                new RoundRobinScheduler()
            );
    }
 
    return std::unique_ptr<Scheduler>();
}
