#include "energy_assignment.h"
#include <iostream>
#include <cmath>

int MATRIX_1_A[NUMBER_OF_TASKS_1_A][TUPLA] = {{1, 2, 3}, {1, 2, 4}, {1, 2, 2}};
int MATRIX_1_B[NUMBER_OF_TASKS_1_B][TUPLA] = {{3, 5, 2}, {1, 3, 3}, {1, 3, 3}, {1, 3, 5}};
int MATRIX_1_C[NUMBER_OF_TASKS_1_C][TUPLA] = {{7, 10, 10}};
int MATRIX_1_D[NUMBER_OF_TASKS_1_D][TUPLA] = {{8, 15, 10}, {10, 15, 12}};
int MATRIX_1_E[NUMBER_OF_TASKS_1_E][TUPLA] = {{15, 20, 10}};
int MATRIX_1_PLAN_EMERGENCIA[NUMBER_OF_TASKS_PLAN_EMERGENCIA][TUPLA] = {{18, 20, 2}};                                                                                      

// definition of quality levels
float QoS[NUMBER_OF_PLANS_1] = {100, 87.5, 75, 62.5, 50, 70}; //QoS in percentage


float D[MONTHS] = {84.58, 123.33, 178.75, 212.92, 247.92, 295.42, 300.00, 264.17, 202.92, 130.42, 88.75, 70.83};
int STD_HOURS[MONTHS] = {8, 8, 8, 10, 11, 13, 13, 13, 12, 9, 8, 8};
int MIN_HOUR[MONTHS] = {8, 8, 8, 8, 7, 7, 7, 7, 7, 8, 8, 8};
int MAX_HOUR[MONTHS] = {16, 16, 16, 18, 18, 20, 20, 20, 19, 17, 16, 16};

//cost of all the plans
float cost_of_plan[NUMBER_OF_PLANS_1];
float vector_efficiency[NUMBER_OF_PLANS_1];
int ddMMyyhhmmss[6];
int plans[NUMBER_OF_PLANS_1];
int assignments[SLOTS];
double battery_at_slots[SLOTS + 1];
 
assignmentClass::assignmentClass(void)
{
    battery_at_slots[0] = CAPACITY - 500;

    cost_of_plan[0] = compute_cost_scheduling_plan(MATRIX_1_A, NUMBER_OF_TASKS_1_A);
    cost_of_plan[1] = compute_cost_scheduling_plan(MATRIX_1_B, NUMBER_OF_TASKS_1_B);
    cost_of_plan[2] = compute_cost_scheduling_plan(MATRIX_1_C, NUMBER_OF_TASKS_1_C);
    cost_of_plan[3] = compute_cost_scheduling_plan(MATRIX_1_D, NUMBER_OF_TASKS_1_D);
    cost_of_plan[4] = compute_cost_scheduling_plan(MATRIX_1_E, NUMBER_OF_TASKS_1_E);
    cost_of_plan[5] = compute_cost_scheduling_plan(MATRIX_1_PLAN_EMERGENCIA, NUMBER_OF_TASKS_PLAN_EMERGENCIA);
	
    for (int i = 0; i < NUMBER_OF_PLANS_1; i++)
		std::cout << "El plan " << i << " tiene un coste de " << cost_of_plan[i] << std::endl;
}

int* assignmentClass::assign_plan(int month)
{
    ddMMyyhhmmss[0] = 1;
    ddMMyyhhmmss[1] = month;
    ddMMyyhhmmss[2] = 2012;
    ddMMyyhhmmss[3] = 0;
    ddMMyyhhmmss[4] = 0;
    ddMMyyhhmmss[5] = 0;
	
    int hour = 0;
    int x, r1, r2;
    compute_efficiency(QoS, cost_of_plan, vector_efficiency, NUMBER_OF_PLANS_1);
    order_plans_by_efficiency(vector_efficiency, plans, NUMBER_OF_PLANS_1);
    get_time_from_seconds(0, ddMMyyhhmmss);

    r1 = look_for_slot(SUNRISE_SLOT);
    r2 = look_for_slot(SUNSET_SLOT);
    x = rand() % (r2 - r1 + 1) + r1;

    for (int i = 0; i < SLOTS; i++)
        assignments[i] = plans[0];
	
    compute_cost_assignment(plans[0], cost_of_plan, ddMMyyhhmmss, battery_at_slots, &hour, 1, ddMMyyhhmmss[1] - 1);

    int optimal = 0, admisible = 0, n = NUMBER_OF_PLANS_1;

    while ((!optimal) && (!admisible))
    {
        if (battery_at_slots[SLOTS] >= battery_at_slots[0])
        {
            if (battery_at_slots[SLOTS] == battery_at_slots[0])
            {
                optimal = 1;
                break;
            }

            n = remove_plan_quality(plans[0], vector_efficiency, plans, n);
            if (n == 0)
            {
                optimal = 1;
                break;
            }
	    std::cout << "Hago upgrade" << std::endl;
            upgrade(plans[0], assignments, battery_at_slots, cost_of_plan, QoS, ddMMyyhhmmss[1] - 1);
        }
        else 
        {
            n = remove_plan_cost(plans[0], cost_of_plan, vector_efficiency, plans, n);
            if (n == 0)
            {
                admisible = 1;
                break;
            }
	    std::cout << "Hago downgrade" << std::endl;
            downgrade(plans[0], assignments, battery_at_slots, cost_of_plan, QoS, ddMMyyhhmmss[1] - 1);
        }
    }

    compute_qos_assignment(assignments, QoS, ddMMyyhhmmss[1]);

    compute_efficiency(QoS, cost_of_plan, vector_efficiency, NUMBER_OF_PLANS_1);
    order_plans_by_efficiency(vector_efficiency, plans, NUMBER_OF_PLANS_1);

    if (reoptimization(assignments, x, battery_at_slots, ddMMyyhhmmss[1] - 1, cost_of_plan, QoS, plans) == -1)
	std::cout << "Not admissible solution" << std::endl;
	
    return assignments;
}

int assignmentClass::compute_time_idle_task(int matrix[][TUPLA], int n)
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
	if (ddMMyyhhmmss[1] == 0) ddMMyyhhmmss[1] = 12;

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

    for (i = 0; i < n; i++)
    {
        if (cost_of_plan[i] < cost_of_plan[plan])
        {
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
	float ep_slot_remaining;
	float EP = 0.0;
	double consumption = 0.0;
	float K = D[month];
	float P = STD_HOURS[month] / K;
	char filename[256];

	// register results
	FILE *fs;
	sprintf(filename, "%s-%d.csv", "results/initial_assignment", month);
	fs = fopen(filename, "w");
	fprintf(fs, "slot,date,plan,QoS,battery,consumption,ep_slot\n");

	//instantaneous energy production -->
	//we compute the energy produced in every month and...
	EP = (D[month] * (EFFICIENCY / 100) * S) / Vld; //energy produced in the hour i IN Amperes
	EP = EP * 1000;									//energy produced in milliAmperes

	if ((*hour >= MIN_HOUR[month]) && (*hour < MAX_HOUR[month]))
	{
		ep_hour = (-1 * (*hour - H)) * (*hour - H) * (EP / (4 * STD_HOURS[month])) + EP;
		ep_slot = ep_hour / (SLOTS / 24);
		ep_slot_remaining = ep_hour - (ep_slot * (SLOTS / 24));
	}
	else
	{
		ep_slot = 0;
		ep_slot_remaining = 0;
	}

	for (i = init; i <= SLOTS; i++)
	{
		//in every month we compute the irradiance in the month and we distribute in
		if (ddMMyyhhmmss[3] != *hour)
		{
			//the new hour
			*hour = ddMMyyhhmmss[3];

			//is the average of the month and the correction factor due to the hour
			if ((*hour >= MIN_HOUR[month]) && (*hour < MAX_HOUR[month]))
			{
				ep_hour = (-1 * (*hour - H)) * (*hour - H) * (EP / (4 * STD_HOURS[month])) + EP;
				// ep_hour is now distributed in the slots in an hour
				ep_slot = ep_hour / (SLOTS / 24);
				ep_slot_remaining = ep_hour - (ep_slot * (SLOTS / 24));
			}
			else
			{
				ep_slot = 0;
				ep_slot_remaining = 0;
			}
		}
		if (i == SLOTS)
			ep_slot += ep_slot_remaining;

		consumption = (cost_of_plan[plan] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
		// compute the battery level at slot i
		battery_level = battery_at_slots[i - 1] + (EFFICIENCY / 100) * rectifier(ep_slot, consumption) - rectifier(consumption, ep_slot);
		if (battery_level > CAPACITY)
			battery_at_slots[i] = CAPACITY;
		else
			battery_at_slots[i] = battery_level;

		// register data to file
		fprintf(fs, "%d,", i);
		fprintf(fs, "%d-%d-%d:%d:%d:%d,", ddMMyyhhmmss[0], ddMMyyhhmmss[1], ddMMyyhhmmss[2], ddMMyyhhmmss[3], ddMMyyhhmmss[4], ddMMyyhhmmss[5]);
		fprintf(fs, "%d,", plan);
		fprintf(fs, "%f,", QoS[plan]);
		fprintf(fs, "%f,", battery_at_slots[i]);
		fprintf(fs, "%f,", consumption);
		fprintf(fs, "%f\n", ep_slot);
		
		get_time_from_seconds(TIME_SLOTS, ddMMyyhhmmss);
	}
	fclose(fs);
}

void assignmentClass::recompute_battery_level(int plan, int *assignments, double *battery_at_slots, float *cost_of_plan, int s, int hour, float *QoS, int month, int optimization)
{
    double battery_level, consumption = 0;
	int i;
	float ep_hour = 0.0;
	float ep_slot = 0.0;
	float ep_slot_remaining;
	float EP = 0.0;
	float K = D[month];
	float P = STD_HOURS[month] / K;
	char filename_assignment[256], filename_repotimization[256];
	FILE *fs, *fr;
	// debe estar la hora a 0!!
	int new_date[6] = {1, month + 1, 2012, 0, 0, 0};
	EP = (D[month] * (EFFICIENCY / 100) * S) / Vld; //energy produced in the hour i IN Amperes
	EP = EP * 1000;									//energy produced in milliAmperes

	if ((hour >= MIN_HOUR[month]) && (hour < MAX_HOUR[month]))
	{
		ep_hour = (-1 * (hour - H)) * (hour - H) * (EP / (4 * STD_HOURS[month])) + EP;
		if (optimization)
			ep_slot = (ep_hour + (ep_hour * COEFFICIENT)) / (SLOTS / 24);
		else
			ep_slot = ep_hour / (SLOTS / 24);
		
		ep_slot_remaining = ep_hour - (ep_slot * (SLOTS / 24));
	}
	else
	{
		ep_slot = 0;
		ep_slot_remaining = 0;
	}

	sprintf(filename_assignment, "%s-%d.csv", "results/initial_assignment", month);
	fs = fopen(filename_assignment, "a");

	sprintf(filename_repotimization, "%s-%d-%f.csv", "results/reoptimization", month, COEFFICIENT);
	fr = fopen(filename_repotimization, "a");
	// compute the production at this slot
	
	for (i = 1; i <= SLOTS; i++)
	{

		consumption = (cost_of_plan[assignments[i - 1]] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
		if (i >= s)
		{
			if (new_date[3] != hour)
			{
				//the new hour
				hour = new_date[3];

				//is the average of the month and the correction factor due to the hour
				if ((hour >= MIN_HOUR[month]) && (hour < MAX_HOUR[month]))
				{
					ep_hour = (-1 * (hour - H)) * (hour - H) * (EP / (4 * STD_HOURS[month])) + EP;
					if (optimization)
						ep_slot = (ep_hour + (ep_hour * COEFFICIENT)) / (SLOTS / 24);
					else
						ep_slot = ep_hour / (SLOTS / 24);
					ep_slot_remaining = ep_hour - (ep_slot * (SLOTS / 24));
				}
				else
				{
					ep_slot = 0;
					ep_slot_remaining = 0;
				}
			}
			if (i == SLOTS)
				ep_slot += ep_slot_remaining;

			// The assignment[i-1] contains the scheduling plan in battery_at_slots[i]
			battery_level = battery_at_slots[i - 1] + (EFFICIENCY / 100) * rectifier(ep_slot, consumption) - rectifier(consumption, ep_slot);
			if (battery_level > CAPACITY)
				battery_at_slots[i] = CAPACITY;
			else
				battery_at_slots[i] = battery_level;

			// register data to file
			if (optimization)
			{
				fprintf(fr, "%d,", i);
				fprintf(fr, "%f,", ep_slot);
				if (ep_slot > consumption)
					fprintf(fr, "%s\n", "superhabit");
				else
					fprintf(fr, "%s\n", "deficit");
			}
		}

		fprintf(fs, "%d,", i);
		fprintf(fs, "%d-%d-%d:%d:%d:%d,", new_date[0], new_date[1], new_date[2], new_date[3], new_date[4], new_date[5]);
		fprintf(fs, "%d,", assignments[i - 1]);
		fprintf(fs, "%f,", QoS[assignments[i - 1]]);
		fprintf(fs, "%f,", battery_at_slots[i]);
		fprintf(fs, "%f,", consumption);
		if ((new_date[3] >= MIN_HOUR[month]) && (new_date[3] < MAX_HOUR[month]))
			fprintf(fs, "%f\n", ep_slot);
		else
			fprintf(fs, "%f\n", 0.0);

		
		get_time_from_seconds(TIME_SLOTS, new_date);
	}
	fclose(fs);
	fclose(fr);
}

void assignmentClass::upgrade(int plan, int *assignments, double *battery_at_slots, float *cost_of_plan, float *QoS, int month)
{
    int i = 0;
	int s;
	double consumption_new_plan = 0.0;
	double consumption_old_plan = 0.0;
	int old;

	// we compute the consumption in each slot for all the tasks: consumption(i)*duration_slot(i)*voltage
	s = look_for_slot(SUNRISE_SLOT);
	consumption_new_plan = (cost_of_plan[plan] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
	consumption_old_plan = (cost_of_plan[assignments[s - 1]] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
	while (((battery_at_slots[SLOTS] - battery_at_slots[0]) >= (consumption_new_plan - consumption_old_plan)) && (i < SLOTS))
	{
		old = assignments[s - 1];
		consumption_old_plan = (cost_of_plan[old] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
		assignments[s - 1] = plan;
		recompute_battery_level(plan, assignments, battery_at_slots, cost_of_plan, s, SUNRISE_SLOT, QoS, month, 0);
		s = (s % SLOTS) + 1;
		i++;
	}
}

void assignmentClass::downgrade(int plan, int *assignments, double *battery_at_slots, float *cost_of_plan, float *QoS, int month)
{
    int i = 0;
	int s;
	int old;
	s = look_for_slot(SUNSET_SLOT);
	while (((battery_at_slots[SLOTS] - battery_at_slots[0]) < 0) && (i < SLOTS))
	{
		old = assignments[s - 1];
		assignments[s - 1] = plan;
		recompute_battery_level(plan, assignments, battery_at_slots, cost_of_plan, s, SUNSET_SLOT, QoS, month, 0);
		s = (s % SLOTS) + 1;
		i++;
	}
}

void assignmentClass::compute_qos_assignment(int *assignments, float *QoS, int month) 
{
    int i = 0;
    float qos_assignment = 0.0;

    for (i = 0; i < SLOTS; i++)
        qos_assignment += QoS[assignments[i]];
}

int assignmentClass::reoptimization(int *assignments, int x, double *battery_at_slots, int month, float *cost_of_plan, float *QoS, int *plans)
{
	int i = 0, s, plan, plan_old, j, k;
	int replaced = 1;
	float consumption_new, consumption_old;
	int assignment_new[SLOTS];
	char filename[256];
	int ddMMyyhhmmss[6];

	FILE *fr;
	sprintf(filename, "%s-%d-%f.csv", "results/reoptimization", month, COEFFICIENT);
	fr = fopen(filename, "w");
	fprintf(fr, "slot,ep_slot,variation\n");
	fclose(fr);

	for (i = 0; i < SLOTS; i++)
		assignment_new[i] = assignments[i];

	look_for_time(x, ddMMyyhhmmss);
	recompute_battery_level(0, assignments, battery_at_slots, cost_of_plan, x + 1, ddMMyyhhmmss[3], QoS, month, 1);
	printf("after modifying the production at slot=%d battery_at_slots[SLOTS]=%f\t\n", x, battery_at_slots[SLOTS]);

	if (battery_at_slots[SLOTS] >= (battery_at_slots[0] + EPSILON))
	{
		replaced = 1;
		plan_old = assignments[x];
		j = 0;
		while ((plan_old != plans[j]) && (j < NUMBER_OF_PLANS_1))
			j++;
		if (j >= NUMBER_OF_PLANS_1 - 1)
			return -1;
		j++;
		k = plans[j];

		// the next plan in the scheduling plan
		while (replaced)
		{
			replaced = 0;
			s = x + 1;

			while (s <= SLOTS)
			{
				plan_old = assignments[s - 1];
				consumption_old = (cost_of_plan[plan_old] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
				consumption_new = (cost_of_plan[k] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
				
				if ((k < NUMBER_OF_PLANS_1 - 1) && (k >= 0) && ((battery_at_slots[SLOTS] - battery_at_slots[0]) >= (consumption_new - consumption_old)))
				{
					assignment_new[s - 1] = k;
					recompute_battery_level(k, assignment_new, battery_at_slots, cost_of_plan, s, ddMMyyhhmmss[3], QoS, month, 1);
					replaced = 1;
					get_time_from_seconds(TIME_SLOTS, ddMMyyhhmmss);
				}
				s++;
			}
			j++;
			if (j <= NUMBER_OF_PLANS_1 - 1)
			{
				k = plans[j];
			} else {
				replaced = 0;
			}
		}
	}
	else
	{
		plan_old = assignments[x];
		plan = plan_old;
		j = 0;
		while ((plan_old != plans[j]) && (j < NUMBER_OF_PLANS_1))
			j++;
		j--;
		if (j == 0)
			return -1;
		k = plans[j];
		replaced = 1;
		while (replaced)
		{
			s = x + 1;
			replaced = 0;
			while (s <= SLOTS)
			{
				if((k >= 1) && (battery_at_slots[SLOTS] < battery_at_slots[0]))
				{
					assignment_new[s - 1] = k;
					recompute_battery_level(k, assignment_new, battery_at_slots, cost_of_plan, s, ddMMyyhhmmss[3], QoS, month, 1);
					get_time_from_seconds(TIME_SLOTS, ddMMyyhhmmss);
					replaced = 1;
				}
				s++;
			}
			plan_old = k;
			j--;
			if (j < 0)
				replaced = 0;
			else 
				k = plans[j];
		}
	}

	for (i = 0; i < SLOTS; i++)
		assignments[i] = assignment_new[i];

	if (battery_at_slots[SLOTS] < battery_at_slots[0])
		return -1;

	return 0;
}
