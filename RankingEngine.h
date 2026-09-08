
#pragma once
 
#include <vector>
 
#include "BenchmarkResult.h"
 
class RankingEngine
{
public:
    std::vector<BenchmarkResult> rank(
        const std::vector<BenchmarkResult>& results
    ) const;
 
private:
    double calculateScore(
        const BenchmarkResult& result
    ) const;
};
 
