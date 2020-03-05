#include "energy_assignment.h"
#include <iostream>
#include <cmath>

int MATRIX_1_A[NUMBER_OF_TASKS_1_A][TUPLA] = {{2, 5, 10}, {1, 5, 15}, {1, 5, 8}};
int MATRIX_1_B[NUMBER_OF_TASKS_1_B][TUPLA] = {{1, 10, 3}, {1, 10, 5}, {1, 10, 5}, {1, 10, 7}};
int MATRIX_1_C[NUMBER_OF_TASKS_1_C][TUPLA] = {{5, 15, 10}};
int MATRIX_1_D[NUMBER_OF_TASKS_1_D][TUPLA] = {{2, 20, 3}, {4, 20, 4}};
int MATRIX_1_E[NUMBER_OF_TASKS_1_E][TUPLA] = {{20, 30, 10}};
int MATRIX_1_PLAN_EMERGENCIA[NUMBER_OF_TASKS_PLAN_EMERGENCIA][TUPLA] = {{26, 30, 2}};

// definition of quality levels
float QoS[NUMBER_OF_PLANS_1] = {100, 90, 85, 80, 75, 70}; //QoS in percentage


float D[MONTHS] = {84.58, 123.33, 178.75, 212.92, 247.92, 295.42, 300.00, 264.17, 202.92, 130.42, 88.75, 70.83};
int STD_HOURS[MONTHS] = {8, 8, 8, 10, 11, 13, 13, 13, 12, 9, 8, 8};
int MIN_HOUR[MONTHS] = {8, 8, 8, 8, 7, 7, 7, 7, 7, 8, 8, 8};
int MAX_HOUR[MONTHS] = {16, 16, 16, 18, 18, 20, 20, 20, 19, 17, 16, 16};

int compute_time_idle_task(int matrix[][TUPLA], int n)
{
    int time_idle_task = 0;
    int sum = 0;
    int i = 0;

    for (i = 0; i < n; i++)
        sum += matrix[i][0];

    time_idle_task = matrix[0][1] - sum;
    return time_idle_task;
}

float assignmentClass::compute_cost_scheduling_plan(int matrix[][TUPLA], int n)
{
    float cost = 0.0;
    float cost_idle_task = 0.0;
    float cost_other_tasks = 0.0;
    int i = 0;

    for (i = 0; i < n; i++)
    {
        //std::cout << "olaaa";
        //printf("Tarea %d=(%d, %d, %d\n", i, matrix[i][0], matrix[i][1], matrix[i][2]);
        cost_other_tasks += ((float)matrix[i][2] / (float)matrix[i][1]);
        cost_idle_task += ((float)matrix[i][0] / (float)matrix[i][1]);
    }
    cost = cost_other_tasks + ENERGY_IDLE_TASK * (1 - cost_idle_task);
    return cost;
}

void assignmentClass::compute_efficiency(float *qos, float *cost_of_plan, float *vector_efficiency, int n)
{
    int i = 0;

    for (i = 0; i < n; i++)
        vector_efficiency[i] = (float)(qos[i] / cost_of_plan[i]);
}

void assignmentClass::order_plans_by_efficiency(float *vector_efficiency, int *plans, int n)
{
    int i = 0, j = 0;
    int change;
    float aux;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n - i; j++)
        {
            if (vector_efficiency[j + 1] > vector_efficiency[j])
            {
                change = j + 1;
                aux = vector_efficiency[j + 1];
                vector_efficiency[j + 1] = vector_efficiency[j];
                vector_efficiency[j] = aux;
            }
        }
        plans[i] = change;
    }
}

void assignmentClass::get_time_from_seconds(int seconds, int *ddMMyyhhmmss)
{
	int resto, resto1;

	resto = (ddMMyyhhmmss[5] + seconds) / 60;
	ddMMyyhhmmss[5] = (ddMMyyhhmmss[5] + seconds) % 60;

	resto1 = (resto + ddMMyyhhmmss[4]) / 60;
	ddMMyyhhmmss[4] = (resto + ddMMyyhhmmss[4]) % 60;

	resto = (resto1 + ddMMyyhhmmss[3]) / 60;
	ddMMyyhhmmss[3] = (resto1 + ddMMyyhhmmss[3]) % 24;

	resto1 = (resto + ddMMyyhhmmss[0]) / 30;
	ddMMyyhhmmss[0] = (resto + ddMMyyhhmmss[0]) % 30;

	resto = (resto1 + ddMMyyhhmmss[1]) / 12;
	ddMMyyhhmmss[1] = (resto1 + ddMMyyhhmmss[1]) % 12;

	if (resto > 1)
		ddMMyyhhmmss[2]++;
}

double assignmentClass::rectifier(double a, double b)
{
    double res = 0.0;
    res = a - b;

    if (res >= 0)
        return res;
    
    return 0.0;
}

int assignmentClass::remove_plan_quality(int plan, float *vector_efficiency, int *plans, int n)
{
    int i = 0, j = 0;
    float vector_efficiency_aux[NUMBER_OF_PLANS_1];

    for (i = 0; i < n; i++)
    {
        if (QoS[plans[i]] > QoS[plan])
        {
            //print
            vector_efficiency_aux[j] = vector_efficiency[i];
            plans[j] = plans[i];
            j++;
        }
    }
    for (i = 0; i < j; i++)
        vector_efficiency[i] = vector_efficiency_aux[i];
    
    return j;
}

int assignmentClass::remove_plan_cost(int plan, float *cost_of_plan, float *vector_efficiency, int *plans, int n)
{
    int i, j = 0;
    float vector_efficiency_aux[NUMBER_OF_PLANS_1];

    //print
    for (i = 0; i < n; i++)
    {
        if (cost_of_plan[i] < cost_of_plan[plan])
        {
            //print
            vector_efficiency_aux[j] = vector_efficiency[i];
            plans[j] = i;
            j++;
        }
    }
    for (i = 0; i < j; i++)
        vector_efficiency[i] = vector_efficiency_aux[i];

    return j;
}

void assignmentClass::look_for_time(int slot, int *ddMMyyhhmmss)
{
    int hour, min, seconds;

    slot = slot * TIME_SLOTS;
    hour = slot / 3600;
    min = (slot % 3600) / 60;
    seconds = (slot % 3600) % 60;

    ddMMyyhhmmss[0] = 1;
    ddMMyyhhmmss[1] = 0;
    ddMMyyhhmmss[2] = 2012;
    ddMMyyhhmmss[3] = hour;
    ddMMyyhhmmss[4] = min;
    ddMMyyhhmmss[5] = seconds;
}

int assignmentClass::look_for_slot(int time)
{
    int slot; 

    slot = ceil(SLOTS / 24) * time;
    return slot;
}

void assignmentClass::compute_cost_assignment(int plan, float *cost_of_plan, int *ddMMyyhhmmss, double *battery_at_slots, int *hour, int init, int month)
{
    int i;
    double battery_level;
    float ep_hour = 0.0;
    float ep_slot = 0.0;
    float ep_slot_remaining = 0.0;
    float EP = 0.0;
    double consumption = 0.0;
    float K = D[month];
    float P = STD_HOURS[month] / K;
    char filename[256];

    EP = (K * (EFFICIENCY / 100) * S) / Vld;
    EP *= 1000;

    if ((*hour >= MIN_HOUR[month]) && (*hour < MAX_HOUR[month]))
    {
        ep_hour = (-1 * (*hour - H)) * (*hour - H) * (EP / (4 * STD_HOURS[month])) + EP;
        ep_slot = ep_hour / (SLOTS / 24);
        ep_slot_remaining = ep_hour - (ep_slot * (SLOTS / 24));
    } else {
        ep_slot = 0;
        ep_slot_remaining = 0;
    }

    for (i = init; i <= SLOTS; i++)
    {
        if (ddMMyyhhmmss[3] != *hour)
        {
            *hour = ddMMyyhhmmss[3];

            if ((*hour >= MIN_HOUR[month]) && (*hour < MAX_HOUR[month]))
            {
                ep_hour = (-1 * (*hour - H)) * (*hour - H) * (EP / (4 * STD_HOURS[month])) + EP;
                ep_slot = ep_hour / (SLOTS / 24);
                ep_slot_remaining = ep_hour - (ep_slot * (SLOTS / 24));
            } else {
                ep_slot = 0;
                ep_slot_remaining = 0;
            }
        }
        if (i == SLOTS)
            ep_slot += ep_slot_remaining;

        consumption = (cost_of_plan[plan] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
        battery_level = battery_at_slots[i - 1] + (EFFICIENCY / 100) * rectifier(ep_slot, consumption) - rectifier(consumption, ep_slot);
        if (battery_level > CAPACITY)
            battery_at_slots[i] = CAPACITY;
        else
            battery_at_slots[i] = battery_level;

        //prints
    }
}

void assignmentClass::recompute_battery_level(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, int s, int hour, float *QoS, int month, int optimization)
{
    double battery_level, consumption = 0;
    int i;
    float ep_hour = 0.0;
    float ep_slot = 0.0;
    float ep_slot_remaining;
    float EP = 0.0;
    float K = D[month];
    float P = STD_HOURS[month] / K;
    char filename[256];

    int new_date[6] = {1, month + 1, 2012, 0, 0, 0};
    EP = (K * (EFFICIENCY / 100) * S) / Vld;
    EP *= 1000;

    if ((hour >= MIN_HOUR[month]) && (hour < MAX_HOUR[month]))
    {
        ep_hour = (-1 * (hour - H)) * (hour - H) * (EP / (4 * STD_HOURS[month])) + EP;
        if (optimization)
            ep_slot = (ep_hour + (ep_hour * COEFFICIENT)) / (SLOTS / 24);
        else 
            ep_slot = ep_hour / (SLOTS / 24);
        
        ep_slot_remaining = ep_hour - (ep_slot * (SLOTS / 24));
    } else {
        ep_slot = 0;
        ep_slot_remaining = 0;
    }

    for (i = 1; i <= SLOTS; i++)
    {
        consumption = (cost_of_plan[assignment[i - 1]] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE / BATTERIES);
        if (i >= s)
        {
            if (new_date[3] != hour)
            {
                hour = new_date[3];

                if ((hour >= MIN_HOUR[month]) && (hour < MAX_HOUR[month]))
                {
                    ep_hour = (-1 * (hour - H)) * (hour - H) * (EP / (4 * STD_HOURS[month])) + EP;
                    if (optimization)
                        ep_slot = (ep_hour + (ep_hour * COEFFICIENT)) / (SLOTS / 24);
                    else
                        ep_slot = ep_hour / (SLOTS / 24);

                    ep_slot_remaining = ep_hour - (ep_slot * (SLOTS / 24));
                }
            }
            if (i == SLOTS)
                ep_slot += ep_slot_remaining;

            battery_level = battery_at_slots[i - 1] + (EFFICIENCY / 100) * rectifier(ep_slot, consumption) - rectifier(consumption, ep_slot);
            if (battery_level > CAPACITY)
                battery_at_slots[i] = CAPACITY;
            else 
                battery_at_slots[i] = battery_level;
        }
    //prints
    }
}

void assignmentClass::upgrade(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, float *QoS, int month)
{
    int i = 0, s;
    double consumption_new_plan = 0.0;
    double consumption_old_plan = 0.0;
    int old;

    s = look_for_slot(SUNRISE_SLOT);
    consumption_new_plan = (cost_of_plan[plan] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
    consumption_old_plan = (cost_of_plan[assignment[s - 1]] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
    while (((battery_at_slots[SLOTS] - battery_at_slots[0]) >= (consumption_new_plan - consumption_old_plan)) && (i < SLOTS))
    {
        old = assignment[s - 1];
        consumption_old_plan = (cost_of_plan[old] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
        assignment[s - 1] = plan;
        recompute_battery_level(plan, assignment, battery_at_slots, cost_of_plan, s, SUNRISE_SLOT, QoS, month, 0);
        s = (s % SLOTS) + 1;
        i++;
    }
}

void assignmentClass::downgrade(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, float *QoS, int month)
{
    int old, s, i = 0;
    s = look_for_slot(SUNSET_SLOT);
    while (((battery_at_slots[SLOTS] - battery_at_slots[0]) < 0) && (i < SLOTS))
    {
        old = assignment[s - 1];
        assignment[s - 1] = plan;
        recompute_battery_level(plan, assignment, battery_at_slots, cost_of_plan, s, SUNSET_SLOT, QoS, month, 0);
        s = (s % SLOTS) + 1;
        i++;
    }
}

void assignmentClass::compute_qos_assignment(int *assignment, float *QoS, int month) 
{
    int i = 0;
    float qos_assignment = 0.0;

    for (i = 0; i < SLOTS; i++)
        qos_assignment += QoS[assignment[i]];
}