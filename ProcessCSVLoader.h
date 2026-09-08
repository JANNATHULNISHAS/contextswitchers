
#pragma once

#include <string>
#include <vector>

#include "ProcessModel.h"

class ProcessCSVLoader
{
public:

    std::vector<ProcessModel> load(
        const std::string& filePath)const;
};
