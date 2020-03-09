#include <iostream>
#include <fstream>
#include "energy_assignment.h"

int main()
{
    //std::cout << "holaa\n";
    //std::cout << "Esto es una prueba %d:%d" << 1 << 2 << std::endl;
    //fprintf()
    assignmentClass assignment = assignmentClass();
    int *assignments = assignment.assign_plan();
    return 0;
}