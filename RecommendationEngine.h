
#pragma once
 
#include "RecommendationResult.h"
#include "WorkloadMetrics.h"
 
class RecommendationEngine
{
public:
    RecommendationResult recommend(
        const WorkloadMetrics& metrics
    ) const;
 
private:
    RecommendationResult recommendFCFS(
        const WorkloadMetrics& metrics
    ) const;
 
    RecommendationResult recommendSJF(
        const WorkloadMetrics& metrics
    ) const;
 
    RecommendationResult recommendSRTF(
        const WorkloadMetrics& metrics
    ) const;
 
    RecommendationResult recommendPriority(
        const WorkloadMetrics& metrics
    ) const;
 
    RecommendationResult recommendRoundRobin(
        const WorkloadMetrics& metrics
    ) const;
};
