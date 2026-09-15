#include "ApplicationMenu.h"
 
#include <iostream>
#include <fstream>
#include <string>

 
#include "SimulationManager.h"
 
void ApplicationMenu::run()
{
    bool running = true;
 
    while (running)
    {
        const int choice =
            displayMainMenu();
 
        switch (choice)
        {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                runApplication(choice);
                break;
 
            case 6:
                viewReports();
                break;
 
            case 7:
                viewLogs();
                break;
 
            case 0:
                running = false;
 
                std::cout
                    << std::endl
                    << "Exiting CPU Scheduler Simulator."
                    << std::endl;
 
                break;
 
            default:
                std::cout
                    << std::endl
                    << "Invalid choice. Please try again."
                    << std::endl;
        }
    }
}
 
int ApplicationMenu::displayMainMenu() const
{
    std::cout
        << std::endl
        << "========================================================"
        << std::endl;
 
    std::cout
        << "              CPU SCHEDULER SIMULATOR"
        << std::endl;
 
    std::cout
        << "========================================================"
        << std::endl;
 
    std::cout
        << std::endl
        << "AVAILABLE APPLICATIONS"
        << std::endl;
 
    std::cout
        << "1. Banking"
        << std::endl;
 
    std::cout
        << "2. Batch Processing"
        << std::endl;
 
    std::cout
        << "3. E-Commerce"
        << std::endl;
 
    std::cout
        << "4. Healthcare"
        << std::endl;
 
    std::cout
        << "5. Streaming"
        << std::endl;
 
    std::cout
        << std::endl
        << "--------------------------------------------------------"
        << std::endl;
 
    std::cout
        << "6. View Report Files"
        << std::endl;
 
    std::cout
        << "7. View Log Files"
        << std::endl;
 
    std::cout
        << "0. Exit"
        << std::endl;
 
    std::cout
        << "--------------------------------------------------------"
        << std::endl;
 
    std::cout
        << "Enter your choice: ";
 
    int choice;
 
    std::cin >> choice;
 
    return choice;
}
 
void ApplicationMenu::runApplication(
    int applicationChoice)
{
    const char* applicationName =
        getApplicationName(
            applicationChoice
        );
 
    const char* csvPath =
        getCSVPath(
            applicationChoice
        );
 
    std::cout
        << std::endl
        << "========================================================"
        << std::endl;
 
    std::cout
        << "Selected Application : "
        << applicationName
        << std::endl;
 
    std::cout
        << "Workload Source      : "
        << csvPath
        << std::endl;
 
    std::cout
        << "========================================================"
        << std::endl;
 
    SimulationManager manager;
 
    manager.run(csvPath,applicationName);
 
    std::cout
        << std::endl
        << "--------------------------------------------------------"
        << std::endl;
 
    std::cout
        << "0. Back to Main Menu"
        << std::endl;
 
    std::cout
        << "--------------------------------------------------------"
        << std::endl;
 
    int choice;
 
    std::cin >> choice;
 
    if (choice != 0)
    {
        std::cout
            << "Returning to main menu."
            << std::endl;
    }
}
 
void ApplicationMenu::viewReports() const
{
    std::cout
        << std::endl
        << "========================================================"
        << std::endl;
 
    std::cout
        << "                  REPORT FILE VIEWER"
        << std::endl;
 
    std::cout
        << "========================================================"
        << std::endl;
 
    std::cout
        << std::endl
        << "1. Workload Analysis Report"
        << std::endl;
 
    std::cout
        << "2. Benchmark Report"
        << std::endl;
 
    std::cout
        << "3. Summary Report"
        << std::endl;
 
    std::cout
        << "4. Process Report"
        << std::endl;
 
    std::cout
        << "5. State Transition Report"
        << std::endl;
 
    std::cout
        << "0. Back"
        << std::endl;
 
    std::cout
        << std::endl
        << "Enter your choice: ";
 
    int choice;
    std::cin >> choice;
 
    if (choice == 0)
    {
        return;
    }
 
    const char* filePath = "";
 
    switch (choice)
    {
        case 1:
            filePath = "reports/workload_analysis_report.txt";
            break;
 
        case 2:
            filePath = "reports/benchmark_report.txt";
            break;
 
        case 3:
            filePath = "reports/summary_report.txt";
            break;
 
        case 4:
            filePath = "reports/process_report.csv";
            break;
 
        case 5:
            filePath = "reports/state_transition_report.txt";
            break;
 
        default:
            std::cout
                << std::endl
                << "Invalid report choice."
                << std::endl;
            return;
    }
 
    std::ifstream report(filePath);
 
    if (!report.is_open())
    {
        std::cout
            << std::endl
            << "Unable to open report file:"
            << std::endl
            << filePath
            << std::endl;
 
        return;
    }
 
    std::cout
        << std::endl
        << "========================================================"
        << std::endl;
 
    std::cout
        << "REPORT: "
        << filePath
        << std::endl;
 
    std::cout
        << "========================================================"
        << std::endl
        << std::endl;
 
    std::string line;
 
    while (std::getline(report, line))
    {
        std::cout << line << std::endl;
    }
 
    report.close();
 
    std::cout
        << std::endl
        << "========================================================"
        << std::endl;
 
    std::cout
        << "End of report."
        << std::endl;
 
    std::cout
        << "========================================================"
        << std::endl;
 
    std::cout
        << std::endl
        << "Press Enter to continue...";
 
    std::cin.ignore();
    std::cin.get();
}
 
 
void ApplicationMenu::viewLogs() const
{
    std::cout
        << std::endl
        << "========================================================"
        << std::endl;
 
    std::cout
        << "                    LOG FILE VIEWER"
        << std::endl;
 
    std::cout
        << "========================================================"
        << std::endl;
 
    const char* filePath =
        "logs/execution_log.txt";
 
    std::ifstream logFile(filePath);
 
    if (!logFile.is_open())
    {
        std::cout
            << std::endl
            << "Unable to open log file:"
            << std::endl
            << filePath
            << std::endl;
 
        return;
    }
 
    std::cout
        << std::endl
        << "LOG: "
        << filePath
        << std::endl;
 
    std::cout
        << "--------------------------------------------------------"
        << std::endl;
 
    std::string line;
 
    while (std::getline(logFile, line))
    {
        std::cout << line << std::endl;
    }
 
    logFile.close();
 
    std::cout
        << "--------------------------------------------------------"
        << std::endl;
 
    std::cout
        << "End of log."
        << std::endl;
 
    std::cout
        << "--------------------------------------------------------"
        << std::endl;
 
    std::cout
        << std::endl
        << "Press Enter to continue...";
 
    std::cin.ignore();
    std::cin.get();
}
 
const char* ApplicationMenu::getApplicationName(
    int applicationChoice) const
{
    switch (applicationChoice)
    {
        case 1:
            return "Banking";
 
        case 2:
            return "Batch Processing";
 
        case 3:
            return "E-Commerce";
 
        case 4:
            return "Healthcare";
 
        case 5:
            return "Streaming";
    }
 
    return "Unknown";
}
 
const char* ApplicationMenu::getCSVPath(
    int applicationChoice) const
{
    switch (applicationChoice)
    {
        case 1:
            return "data/banking.csv";
 
        case 2:
            return "data/batch_processing.csv";
 
        case 3:
            return "data/ecommerce.csv";
 
        case 4:
            return "data/healthcare.csv";
 
        case 5:
            return "data/streaming.csv";
    }
 
    return "";
}
 
