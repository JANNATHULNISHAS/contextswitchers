#pragma once
 
#include <map>
#include <string>
#include <vector>
 
#include "AlgorithmType.h"
#include "ISimulationObserver.h"
#include "ProcessModel.h"
 
class LiveDashboard : public ISimulationObserver
{
private:
    struct DashboardProcess
    {
        std::string name;
        int arrivalTime;
        int burstTime;
        int remainingTime;
        int originalPriority;
        int effectivePriority;
        ProcessState state;
        int preemptionCount;
    };

   
 
    std::map<std::string, DashboardProcess> processes;
 
    std::string applicationName;
    AlgorithmType algorithm;
 
    int currentTime;
    int cpuBusyTime;
    int cpuIdleTime;
    int contextSwitches;
    int preemptions;
    int refreshDelayMs;
    //int dashboardHeight;
   // bool alternateScreenActive;
 
public:
    LiveDashboard(
        const std::string& applicationName,
        AlgorithmType algorithm,
        const std::vector<ProcessModel>& processList,
        int refreshDelayMs = 250
    );
 
    virtual ~LiveDashboard();
 
    virtual void onEvent(const SimulationEvent& event);
 
private:
    void refresh(const SimulationEvent& event);
    void clearScreen() const;
    void displayHeader() const;
    void displayCPUStatus() const;
    void displayProcessTable() const;
    void displayReadyQueue() const;
    void displayEvent(const SimulationEvent& event) const;
 
    const char* stateToString(ProcessState state) const;
    const char* algorithmToString(AlgorithmType algorithm) const;
};
 
