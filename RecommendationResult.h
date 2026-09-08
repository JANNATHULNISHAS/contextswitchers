
#pragma once
 
#include <string>
 
#include "AlgorithmType.h"
 
class RecommendationResult
{
private:
    AlgorithmType recommendedAlgorithm;
    std::string reason;
    double confidence;
 
public:
    RecommendationResult(
        AlgorithmType algorithm,
        const std::string& reason,
        double confidence
    );
 
    AlgorithmType getRecommendedAlgorithm() const;
 
    const std::string& getReason() const;
 
    double getConfidence() const;
};
