#pragma once
 
class ApplicationMenu
{
public:
    void run();
 
private:
    int displayMainMenu() const;
 
    void runApplication(
        int applicationChoice
    );
 
    void viewReports() const;
 
    void viewLogs() const;
 
    const char* getApplicationName(
        int applicationChoice
    ) const;
 
    const char* getCSVPath(
        int applicationChoice
    ) const;
};
 
