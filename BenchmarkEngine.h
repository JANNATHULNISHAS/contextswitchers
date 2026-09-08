
#pragma once
 
#include <vector>
 
#include "ProcessModel.h"
#include "BenchmarkResult.h"
 
class BenchmarkEngine
{
public:
    std::vector<BenchmarkResult> benchmark(
        const std::vector<ProcessModel>& processes
    ) const;
 
private:
    BenchmarkResult benchmarkAlgorithm(
        const std::vector<ProcessModel>& processes,
        AlgorithmType algorithm
    ) const;
};
