#include <iostream>
#include <fstream>
#include "energy_assignment.h"

int main()
{
    assignmentClass assignments = assignmentClass();
    for (int i = 1; i <= 12; i++)
        int *plans = assignments.assign_plan(i);
    //return 0;
}