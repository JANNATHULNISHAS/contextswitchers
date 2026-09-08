
#pragma once
 
#include <string>
#include <vector>
#include "ProcessModel.h"
 
class WorkloadLoader
{
public:
    std::vector<ProcessModel> loadCSV(
        const std::string& filePath
    ) const;
};
