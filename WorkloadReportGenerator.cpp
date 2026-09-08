#include "WorkloadReportGenerator.h"

#include <fstream>
#include <iomanip>
#include <map>
#include <string>

using namespace std;

void WorkloadReportGenerator::generate(
    const string& applicationName,
    const string& workloadSource,
    const vector<ProcessModel>& processes,
    const WorkloadMetrics& workloadMetrics,
    const RecommendationResult& recommendation
) const
{
    ofstream out("workload_analysis_report.txt");

    if (!out.is_open())
    {
        return;
    }

    out << "============================================================\n";
    out << "              WORKLOAD ANALYSIS REPORT\n";
    out << "============================================================\n\n";


    // ----------------------------------------------------------
    // 1. WORKLOAD INFORMATION
    // ----------------------------------------------------------
    out << "1. WORKLOAD INFORMATION\n";
    out << "------------------------------------------------------------\n";

    out << left
        << setw(30) << "Application Name"
        << ": " << applicationName << "\n";

    out << left
        << setw(30) << "Workload Source"
        << ": " << workloadSource << "\n";

    out << left
        << setw(30) << "Process Count"
        << ": " << workloadMetrics.getProcessCount() << "\n";

    out << "\n";


    // ----------------------------------------------------------
    // 2. BURST TIME ANALYSIS
    // ----------------------------------------------------------
    out << "2. BURST TIME ANALYSIS\n";
    out << "------------------------------------------------------------\n";

    out << fixed << setprecision(2);

    out << left
        << setw(30) << "Average Burst Time"
        << ": " << workloadMetrics.getAverageBurstTime() << "\n";

    out << left
        << setw(30) << "Minimum Burst Time"
        << ": " << workloadMetrics.getMinimumBurstTime() << "\n";

    out << left
        << setw(30) << "Maximum Burst Time"
        << ": " << workloadMetrics.getMaximumBurstTime() << "\n";

    out << left
        << setw(30) << "Variance Value"
        << ": " << workloadMetrics.getBurstVariance() << "\n";

    out << left
        << setw(30) << "Variance Level"
        << ": "
        << varianceLevelToString(
               workloadMetrics.getVarianceLevel()
           )
        << "\n";

    out << "\n";


    // ----------------------------------------------------------
    // 3. PRIORITY ANALYSIS
    // ----------------------------------------------------------
    out << "3. PRIORITY ANALYSIS\n";
    out << "------------------------------------------------------------\n";

    out << left
        << setw(30) << "Priority Levels"
        << ": " << workloadMetrics.getPriorityLevelCount() << "\n";

    out << left
        << setw(30) << "Priority-1 Processes"
        << ": "
        << workloadMetrics.getHighestPriorityProcessCount()
        << "\n";

    // Calculate priority distribution from ProcessModel.
    // The project uses getOriginalPriority(), not getPriority().
    map<int, int> priorityDistribution;

    for (size_t i = 0; i < processes.size(); ++i)
    {
        priorityDistribution[
            processes[i].getOriginalPriority()
        ]++;
    }

    out << "\n";
    out << "Priority Distribution:\n";

    if (priorityDistribution.empty())
    {
        out << "No priority information available.\n";
    }
    else
    {
        for (map<int, int>::const_iterator it =
                 priorityDistribution.begin();
             it != priorityDistribution.end();
             ++it)
        {
            out << "Priority "
                << it->first
                << " : "
                << it->second
                << " process";

            if (it->second != 1)
            {
                out << "es";
            }

            out << "\n";
        }
    }

    out << "\n";


    // ----------------------------------------------------------
    // 4. ARRIVAL PATTERN ANALYSIS
    // ----------------------------------------------------------
    out << "4. ARRIVAL PATTERN ANALYSIS\n";
    out << "------------------------------------------------------------\n";

    ArrivalPattern arrivalPattern =
        workloadMetrics.getArrivalPattern();

    out << left
        << setw(30) << "Arrival Pattern"
        << ": "
        << arrivalPatternToString(arrivalPattern)
        << "\n";

    out << "Analysis:\n";

    switch (arrivalPattern)
    {
        case ArrivalPattern::BATCH:
            out << "Processes arrive in a relatively concentrated "
                   "time period, indicating batch-style workload "
                   "behavior.\n";
            break;

        case ArrivalPattern::INTERACTIVE:
            out << "Processes arrive at different times, indicating "
                   "interactive or dynamically arriving workload "
                   "behavior.\n";
            break;

        case ArrivalPattern::MIXED:
            out << "The workload contains characteristics of both "
                   "batch and interactive process arrivals.\n";
            break;

        default:
            out << "Arrival pattern could not be determined.\n";
            break;
    }

    out << "\n";


    // ----------------------------------------------------------
    // 5. WORKLOAD CHARACTERISTICS SUMMARY
    // ----------------------------------------------------------
    out << "5. WORKLOAD CHARACTERISTICS SUMMARY\n";
    out << "------------------------------------------------------------\n";

    out << left
        << setw(30) << "Process Count"
        << ": "
        << workloadMetrics.getProcessCount()
        << "\n";

    out << left
        << setw(30) << "Average Burst Time"
        << ": "
        << workloadMetrics.getAverageBurstTime()
        << "\n";

    out << left
        << setw(30) << "Variance Level"
        << ": "
        << varianceLevelToString(
               workloadMetrics.getVarianceLevel()
           )
        << "\n";

    out << left
        << setw(30) << "Priority Levels"
        << ": "
        << workloadMetrics.getPriorityLevelCount()
        << "\n";

    out << left
        << setw(30) << "Arrival Pattern"
        << ": "
        << arrivalPatternToString(
               workloadMetrics.getArrivalPattern()
           )
        << "\n";

    out << left
        << setw(30) << "Workload Type"
        << ": "
        << workloadTypeToString(
               workloadMetrics.getWorkloadType()
           )
        << "\n";

    out << left
        << setw(30) << "I/O Processes"
        << ": "
        << workloadMetrics.getIOProcessCount()
        << "\n";

    out << "\n";


    // ----------------------------------------------------------
    // 6. RECOMMENDATION
    // ----------------------------------------------------------
    out << "6. RECOMMENDATION\n";
    out << "------------------------------------------------------------\n";

    out << left
        << setw(30) << "Recommended Algorithm"
        << ": "
        << algorithmTypeToString(
               recommendation.getRecommendedAlgorithm()
           )
        << "\n";

    out << left
        << setw(30) << "Confidence"
        << ": "
        << recommendation.getConfidence()
        << "%\n";

    out << "\n";

    out << "Reason:\n";
    out << recommendation.getReason() << "\n";

    out << "\n";


    // ----------------------------------------------------------
    // EXPECTED BENEFITS
    // ----------------------------------------------------------
    out << "Expected Benefits:\n";

    switch (recommendation.getRecommendedAlgorithm())
    {
        case AlgorithmType::FCFS:
            out << "- Simple and predictable scheduling.\n";
            out << "- Low scheduling overhead.\n";
            out << "- Suitable for workloads where fairness by "
                   "arrival order is important.\n";
            break;

        case AlgorithmType::SJF:
            out << "- Favors shorter CPU bursts.\n";
            out << "- Can reduce average waiting time.\n";
            out << "- Suitable for workloads with predictable "
                   "short-job characteristics.\n";
            break;

        case AlgorithmType::SRTF:
            out << "- Dynamically favors the process with the "
                   "shortest remaining CPU burst.\n";
            out << "- Can reduce average waiting and turnaround "
                   "time.\n";
            out << "- Responds dynamically to newly arriving "
                   "shorter processes.\n";
            break;

        case AlgorithmType::PRIORITY_NON_PREEMPTIVE:
            out << "- Gives priority to important processes.\n";
            out << "- Avoids interruption after CPU allocation.\n";
            out << "- Provides simple priority-based execution.\n";
            break;

        case AlgorithmType::PRIORITY_PREEMPTIVE:
            out << "- Higher-priority processes can receive CPU "
                   "service immediately.\n";
            out << "- Suitable for priority-critical workloads.\n";
            out << "- Provides faster response for important "
                   "processes.\n";
            break;

        case AlgorithmType::ROUND_ROBIN:
            out << "- Provides time-sharing between processes.\n";
            out << "- Improves fairness among ready processes.\n";
            out << "- Suitable for interactive workloads.\n";
            break;

        default:
            out << "- No specific benefits available.\n";
            break;
    }

    out << "\n";


    // ----------------------------------------------------------
    // 7. CONCLUSION
    // ----------------------------------------------------------
    out << "7. CONCLUSION\n";
    out << "------------------------------------------------------------\n";

    out << "The workload was analyzed before scheduler execution "
           "using process count, burst-time characteristics, "
           "priority distribution, I/O characteristics, and "
           "arrival behavior.\n\n";

    out << "The detected workload type is "
        << workloadTypeToString(
               workloadMetrics.getWorkloadType()
           )
        << " with a "
        << varianceLevelToString(
               workloadMetrics.getVarianceLevel()
           )
        << " burst-time variance level and "
        << arrivalPatternToString(
               workloadMetrics.getArrivalPattern()
           )
        << " arrival pattern.\n\n";

    out << "Based on these characteristics, the recommended "
           "scheduling algorithm is "
        << algorithmTypeToString(
               recommendation.getRecommendedAlgorithm()
           )
        << ".\n";

    out << "\n";
    out << "============================================================\n";

    out.close();
}


// --------------------------------------------------------------
// AlgorithmType -> String
// --------------------------------------------------------------
string WorkloadReportGenerator::algorithmTypeToString(
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

        default:
            return "Unknown";
    }
}


// --------------------------------------------------------------
// ArrivalPattern -> String
// --------------------------------------------------------------
string WorkloadReportGenerator::arrivalPatternToString(
    ArrivalPattern pattern
) const
{
    switch (pattern)
    {
        case ArrivalPattern::BATCH:
            return "Batch";

        case ArrivalPattern::INTERACTIVE:
            return "Interactive";

        case ArrivalPattern::MIXED:
            return "Mixed";

        default:
            return "Unknown";
    }
}


// --------------------------------------------------------------
// VarianceLevel -> String
// --------------------------------------------------------------
string WorkloadReportGenerator::varianceLevelToString(
    VarianceLevel level
) const
{
    switch (level)
    {
        case VarianceLevel::LOW:
            return "Low";

        case VarianceLevel::MEDIUM:
            return "Medium";

        case VarianceLevel::HIGH:
            return "High";

        default:
            return "Unknown";
    }
}


// --------------------------------------------------------------
// WorkloadType -> String
// --------------------------------------------------------------
string WorkloadReportGenerator::workloadTypeToString(
    WorkloadType type
) const
{
    switch (type)
    {
        case WorkloadType::BATCH:
            return "Batch";

        case WorkloadType::INTERACTIVE:
            return "Interactive";

        case WorkloadType::PRIORITY_CRITICAL:
            return "Priority Critical";

        case WorkloadType::MIXED:
            return "Mixed";

        default:
            return "Unknown";
    }
}
