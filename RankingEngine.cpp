#include "RankingEngine.h"
 
#include <algorithm>
#include <vector>
 
double RankingEngine::calculateScore(
    const BenchmarkResult& result) const
{
    double score = 0.0;
 
    /*
     * Lower is better:
     * Waiting Time       = 30%
     * Turnaround Time    = 25%
     * Response Time      = 20%
     * Context Switches   = 10%
     *
     * Higher is better:
     * CPU Utilization    = 15%
     */
    score += result.getAverageWaitingTime() * 0.30;
    score += result.getAverageTurnaroundTime() * 0.25;
    score += result.getAverageResponseTime() * 0.20;
    score += result.getContextSwitches() * 0.10;
 
    score -= result.getCPUUtilization() * 0.15;
 
    return score;
}
 
std::vector<BenchmarkResult>
RankingEngine::rank(
    const std::vector<BenchmarkResult>& results) const
{
    std::vector<BenchmarkResult> rankedResults =
        results;
 
    /*
     * Calculate and store the score for every algorithm.
     */
    for (std::size_t i = 0;
         i < rankedResults.size();
         ++i)
    {
        const double score =
            calculateScore(rankedResults[i]);
 
        rankedResults[i].setRankingScore(score);
    }
 
    /*
     * Sort from lowest score to highest score.
     * Lower score means better overall performance.
     */
    std::sort(
        rankedResults.begin(),
        rankedResults.end(),
        [](
            const BenchmarkResult& first,
            const BenchmarkResult& second)
        {
            return first.getRankingScore() <
                   second.getRankingScore();
        }
    );
 
    /*
     * Assign ranks after sorting.
     */
    for (std::size_t i = 0;
         i < rankedResults.size();
         ++i)
    {
        rankedResults[i].setRank(
            static_cast<int>(i + 1)
        );
    }
 
    return rankedResults;
}
 
