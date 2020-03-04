#include "energy_assignment.h"

int MATRIX_1_A[NUMBER_OF_TASKS_1_A][TUPLA] = {{2, 5, 10}, {1, 5, 15}, {1, 5, 8}};
int MATRIX_1_B[NUMBER_OF_TASKS_1_B][TUPLA] = {{1, 10, 3}, {1, 10, 5}, {1, 10, 5}, {1, 10, 7}};
int MATRIX_1_C[NUMBER_OF_TASKS_1_C][TUPLA] = {{5, 15, 10}};
int MATRIX_1_D[NUMBER_OF_TASKS_1_D][TUPLA] = {{2, 20, 3}, {4, 20, 4}};
int MATRIX_1_E[NUMBER_OF_TASKS_1_E][TUPLA] = {{20, 30, 10}};
int MATRIX_1_PLAN_EMERGENCIA[NUMBER_OF_TASKS_PLAN_EMERGENCIA][TUPLA] = {{26, 30, 2}};

// definition of quality levels
float QoS[NUMBER_OF_PLANS_1] = {100, 90, 85, 80, 75, 70}; //QoS in percentage

// definition of the number of slots
#define TIME_SLOTS 288 //in seconds
#define SLOTS (3600 * 24 / TIME_SLOTS)

float D[MONTHS] = {84.58, 123.33, 178.75, 212.92, 247.92, 295.42, 300.00, 264.17, 202.92, 130.42, 88.75, 70.83};
int STD_HOURS[MONTHS] = {8, 8, 8, 10, 11, 13, 13, 13, 12, 9, 8, 8};
int MIN_HOUR[MONTHS] = {8, 8, 8, 8, 7, 7, 7, 7, 7, 8, 8, 8};
int MAX_HOUR[MONTHS] = {16, 16, 16, 18, 18, 20, 20, 20, 19, 17, 16, 16};