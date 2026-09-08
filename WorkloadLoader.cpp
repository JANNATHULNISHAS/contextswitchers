#include "WorkloadLoader.h"
#include "ProcessCSVLoader.h"
 
std::vector<ProcessModel>
WorkloadLoader::loadCSV(
    const std::string& filePath) const
{
    ProcessCSVLoader loader;
 
    return loader.load(filePath);
}
