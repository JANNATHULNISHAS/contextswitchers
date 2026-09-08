#include "SummaryReportGenerator.h"

#include <ctime>
#include <fstream>
#include <iomanip>
#include <sstream>

void SummaryReportGenerator::generate(
    const std::string& applicationName,
    const std::vector<ProcessModel>& processes,
    const ExecutionResult& executionResult,
    const RecommendationResult& recommendation
) const
{
    std::ofstream file("summary_report.txt");

    if (!file.is_open())
    {
        return;
    }

    file << "========================================\n";
    file << "           SUMMARY REPORT\n";
    file << "========================================\n\n";

    file << "--------------- EXECUTION INFORMATION ---------------\n";

    file << std::left
         << std::setw(28) << "Application Name"
         << ": " << applicationName << "\n";

    file << std::setw(28) << "Selected Algorithm"
         << ": " << algorithmTypeToString(executionResult.getAlgorithm()) << "\n";

    file << std::setw(28) << "Execution Date"
         << ": " << getExecutionDate() << "\n";

    file << std::setw(28) << "Total Processes"
         << ": " << processes.size() << "\n";

    file << std::setw(28) << "Total Simulation Time"
         << ": " << executionResult.getTotalExecutionTime() << "\n";

    file << std::setw(28) << "Scheduling Mode"
         << ": " << getSchedulingMode(executionResult.getAlgorithm()) << "\n";

    if (executionResult.getAlgorithm() == AlgorithmType::ROUND_ROBIN)
    {
        file << std::setw(28) << "Round Robin Quantum"
             << ": Configured in scheduler\n";
    }

    file << std::setw(28) << "Aging Enabled"
         << ": No\n";

    file << std::setw(28) << "Aging Disabled"
         << ": Yes\n";

    file << "\n";

    file << "------------------- PERFORMANCE ---------------------\n";

    file << std::fixed << std::setprecision(2);

    file << std::setw(28) << "Average Waiting Time"
         << ": " << executionResult.getAverageWaitingTime() << "\n";

    file << std::setw(28) << "Average Turnaround Time"
         << ": " << executionResult.getAverageTurnaroundTime() << "\n";

    file << std::setw(28) << "Average Response Time"
         << ": " << executionResult.getAverageResponseTime() << "\n";

    file << std::setw(28) << "CPU Utilization"
         << ": " << executionResult.getCPUUtilization() << "%\n";

    file << std::setw(28) << "Throughput"
         << ": " << executionResult.getThroughput() << "\n";

    file << "\n";

    file << "---------------- EXECUTION STATS -------------------\n";

    file << std::setw(28) << "Completed Processes"
         << ": " << executionResult.getCompletedProcesses().size() << "\n";

    file << std::setw(28) << "Context Switches"
         << ": " << executionResult.getContextSwitches() << "\n";

    file << std::setw(28) << "Preemptions"
         << ": " << executionResult.getPreemptions() << "\n";

    file << std::setw(28) << "CPU Busy Time"
         << ": " << executionResult.getCPUBusyTime() << "\n";

    file << std::setw(28) << "Idle CPU Time"
         << ": " << executionResult.getCPUIdleTime() << "\n";

    file << "\n";

    file << "---------------------- OUTCOME -----------------------\n";

    file << std::setw(28) << "Execution Status"
         << ": ";

    if (executionResult.getCompletedProcesses().size() == processes.size())
    {
        file << "COMPLETED\n";
    }
    else
    {
        file << "PARTIALLY COMPLETED\n";
    }

    file << std::setw(28) << "Performance Characteristics"
         << ": " << getPerformanceCharacteristics(executionResult) << "\n";

    file << std::setw(28) << "Recommended Algorithm"
         << ": " << algorithmTypeToString(
                recommendation.getRecommendedAlgorithm()
            ) << "\n";

    file << std::setw(28) << "Recommendation Confidence"
         << ": " << recommendation.getConfidence() << "%\n";

    file << std::setw(28) << "Overall Result"
         << ": " << getOverallResult(executionResult) << "\n";

    file << "\n";

    file << "--------------------- SUMMARY -----------------------\n";

    file << "The scheduler executed the workload using the selected algorithm.\n";
    file << "The execution completed "
         << executionResult.getCompletedProcesses().size()
         << " out of "
         << processes.size()
         << " processes.\n";

    file << "CPU utilization was "
         << executionResult.getCPUUtilization()
         << "% with a total simulation time of "
         << executionResult.getTotalExecutionTime()
         << " time units.\n";

    file << "The recommendation was "
         << algorithmTypeToString(recommendation.getRecommendedAlgorithm())
         << " with "
         << recommendation.getConfidence()
         << "% confidence.\n";

    file << "\n";
    file << "========================================\n";
    file << "        END OF SUMMARY REPORT\n";
    file << "========================================\n";

    file.close();
}

std::string SummaryReportGenerator::algorithmTypeToString(
    AlgorithmType algorithm
) const
{
    switch (algorithm)
    {
        case AlgorithmType::FCFS:
            return "FCFS";

        case AlgorithmType::SJF:
            return "SJF";

        case AlgorithmType::SRTF:
            return "SRTF";

        case AlgorithmType::PRIORITY_NON_PREEMPTIVE:
            return "Priority Non-Preemptive";

        case AlgorithmType::PRIORITY_PREEMPTIVE:
            return "Priority Preemptive";

        case AlgorithmType::ROUND_ROBIN:
            return "Round Robin";
    }

    return "Unknown";
}

std::string SummaryReportGenerator::getExecutionDate() const
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);

    if (localTime == nullptr)
    {
        return "Unavailable";
    }

    std::ostringstream stream;

    stream << std::setfill('0')
           << (localTime->tm_year + 1900)
           << "-"
           << std::setw(2)
           << (localTime->tm_mon + 1)
           << "-"
           << std::setw(2)
           << localTime->tm_mday;

    return stream.str();
}

std::string SummaryReportGenerator::getSchedulingMode(
    AlgorithmType algorithm
) const
{
    switch (algorithm)
    {
        case AlgorithmType::FCFS:
        case AlgorithmType::SJF:
        case AlgorithmType::PRIORITY_NON_PREEMPTIVE:
            return "Non-Preemptive";

        case AlgorithmType::SRTF:
        case AlgorithmType::PRIORITY_PREEMPTIVE:
        case AlgorithmType::ROUND_ROBIN:
            return "Preemptive / Time-Sliced";
    }

    return "Unknown";
}

std::string SummaryReportGenerator::getPerformanceCharacteristics(
    const ExecutionResult& result
) const
{
    std::ostringstream description;

    double utilization = result.getCPUUtilization();
    double waitingTime = result.getAverageWaitingTime();

    if (utilization >= 90.0 && waitingTime <= 10.0)
    {
        description << "High CPU utilization with low waiting time";
    }
    else if (utilization >= 90.0)
    {
        description << "High CPU utilization with moderate scheduling overhead";
    }
    else if (waitingTime <= 10.0)
    {
        description << "Low waiting time with available CPU idle periods";
    }
    else
    {
        description << "Moderate scheduling performance";
    }

    return description.str();
}

std::string SummaryReportGenerator::getOverallResult(
    const ExecutionResult& result
) const
{
    if (result.getCPUUtilization() >= 90.0 &&
        result.getAverageWaitingTime() <= 10.0)
    {
        return "Efficient execution with strong CPU utilization";
    }

    if (result.getCPUUtilization() >= 90.0)
    {
        return "Successful execution with high CPU utilization";
    }

    return "Successful execution with measurable CPU idle time";
}
