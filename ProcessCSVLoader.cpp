#include "ProcessCSVLoader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

std::vector<ProcessModel> ProcessCSVLoader::load(
    const std::string& filePath
) const
{
    std::ifstream file(filePath.c_str());

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Unable to open CSV file: " + filePath
        );
    }

    std::vector<ProcessModel> processes;

    std::string line;

    // Skip header
    if (!std::getline(file, line))
    {
        return processes;
    }

    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        std::stringstream stream(line);

        std::string processName;
        std::string arrivalTimeText;
        std::string burstTimeText;
        std::string priorityText;
        std::string ioTriggerTimeText;
        std::string ioDurationText;

        std::getline(stream, processName, ',');
        std::getline(stream, arrivalTimeText, ',');
        std::getline(stream, burstTimeText, ',');
        std::getline(stream, priorityText, ',');
        std::getline(stream, ioTriggerTimeText, ',');
        std::getline(stream, ioDurationText, ',');

        if (processName.empty() ||
            arrivalTimeText.empty() ||
            burstTimeText.empty() ||
            priorityText.empty())
        {
            throw std::runtime_error(
                "Invalid CSV row in file: " + filePath
            );
        }

        int arrivalTime = std::stoi(arrivalTimeText);
        int burstTime = std::stoi(burstTimeText);
        int priority = std::stoi(priorityText);

        int ioTriggerTime = -1;
        int ioDuration = 0;

        if (!ioTriggerTimeText.empty())
        {
            ioTriggerTime = std::stoi(ioTriggerTimeText);
        }

        if (!ioDurationText.empty())
        {
            ioDuration = std::stoi(ioDurationText);
        }

        if (arrivalTime < 0 ||
            burstTime <= 0 ||
            priority <= 0)
        {
            throw std::runtime_error(
                "Invalid process values in file: " + filePath
            );
        }

        processes.push_back(
            ProcessModel(
                processName,
                arrivalTime,
                burstTime,
                priority,
                ioTriggerTime,
                ioDuration
            )
        );
    }

    return processes;
}
