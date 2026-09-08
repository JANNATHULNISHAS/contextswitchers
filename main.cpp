#include <iostream>
 
#include "SimulationManager.h"
 
int main()
{
    SimulationManager manager;
 
    manager.run("data/banking.csv");
 
    return 0;
}
