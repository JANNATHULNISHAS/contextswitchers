#include "RecommendationResult.h"
 
RecommendationResult::RecommendationResult(
    AlgorithmType algorithm,
    const std::string& reason,
    double confidence
)
    : recommendedAlgorithm(algorithm),
      reason(reason),
      confidence(confidence)
{
}
 
AlgorithmType
RecommendationResult::getRecommendedAlgorithm() const
{
    return recommendedAlgorithm;
}
 
const std::string&
RecommendationResult::getReason() const
{
    return reason;
}
 
double RecommendationResult::getConfidence() const
{
    return confidence;
}
