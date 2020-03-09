#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CAPACITY 2500
#define BATTERIES 2
#define VOLTAGE 1.5
#define EFFICIENCY 11.38 //energy efficiency of the solar cell
#define S 0.0036		 //surface of the solar cell
#define Vld 3.3			 //specific voltage of load
#define H 12			 //D(t) =(t - h)2/4p+k
#define MONTHS 12		 //we start the execution 1 of January of 2012, at 8:00 am
#define TIME 0 //8:00 am. (hours)
#define SUNRISE_SLOT 6
#define SUNSET_SLOT 18
#define EPSILON 0.0001 // epsilon=10^-4?
#define COEFFICIENT (-0.01)

// filenames
#define INITIAL "results/initial_assignment"
#define MONTHLY_QOS "results/monthly_qos.csv"
#define REOPTIMIZATION "results/reoptimization"

// definition of the applications
#define NUMBER_OF_PLANS_1 6 //mas uno porque incluyo el plan de emergencia
#define NUMBER_OF_TASKS_1_A 3
#define NUMBER_OF_TASKS_1_B 4
#define NUMBER_OF_TASKS_1_C 1
#define NUMBER_OF_TASKS_1_D 2
#define NUMBER_OF_TASKS_1_E 1
#define NUMBER_OF_TASKS_PLAN_EMERGENCIA 1


#define ENERGY_IDLE_TASK 2
#define TUPLA 3

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

// prototypes of functions
int compute_time_idle_task(int matrix[][TUPLA], int n);
float compute_cost_scheduling_plan(int matrix[][TUPLA], int n);
void compute_efficiency(float *qos, float *cost_of_plan, float *vector_efficiency, int n);
void get_time_from_seconds(int seconds, int *ddMMyyhhmmss);
void order_plans_by_efficiency(float *vector_efficiency, int *plans, int n);
int remove_plan_quality(int plan, float *vector_efficiency, int *plans, int n);
int remove_plan_cost(int plan, float *cost_of_plan, float *vector_efficiency, int *plans, int n);
double rectifier(double a, double b);
void upgrade(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, float *QoS, int month);
void downgrade(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, float *QoS, int month);
int look_for_slot(int time);
void look_for_time(int slot, int *ddMMyyhhmmss);
void compute_cost_assignment(int plan, float *cost_of_plan, int *ddMMyyhhmmss, double *battery_at_slots, int *hour, int init, int month);
void recompute_battery_level(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, int s, int hour, float *QoS, int month, int optimization);
void compute_qos_assignment(int *assignment, float *QoS, int month);
int reoptimization(int *assignment, int x, double *battery_at_slots, int month, float *cost_of_plan, float *QoS, int *plans);

int main()
{
    int hour = TIME;
    int n = NUMBER_OF_PLANS_1;
    int x, r1, r2;
    double B = CAPACITY - 500;
    int ddMMyyhhmmss[6];
	int i = 0, j = 0;
	int optimal = 0;
	int admisible = 0;
	double consumption;

    // the algorithm
	int time_idle_task[NUMBER_OF_PLANS_1];
	float cost_of_plan[NUMBER_OF_PLANS_1];
	float vector_efficiency[NUMBER_OF_PLANS_1];
	int plans[NUMBER_OF_PLANS_1];
	int assignment[SLOTS];
	double battery_at_slots[SLOTS + 1];
	battery_at_slots[0] = B; // At slot 0, the battery is the initial

    time_idle_task[0] = compute_time_idle_task(MATRIX_1_A, NUMBER_OF_TASKS_1_A);
	cost_of_plan[0] = compute_cost_scheduling_plan(MATRIX_1_A, NUMBER_OF_TASKS_1_A);
	time_idle_task[1] = compute_time_idle_task(MATRIX_1_B, NUMBER_OF_TASKS_1_B);
	cost_of_plan[1] = compute_cost_scheduling_plan(MATRIX_1_B, NUMBER_OF_TASKS_1_B);
	time_idle_task[2] = compute_time_idle_task(MATRIX_1_C, NUMBER_OF_TASKS_1_C);
	cost_of_plan[2] = compute_cost_scheduling_plan(MATRIX_1_C, NUMBER_OF_TASKS_1_C);
	time_idle_task[3] = compute_time_idle_task(MATRIX_1_D, NUMBER_OF_TASKS_1_D);
	cost_of_plan[3] = compute_cost_scheduling_plan(MATRIX_1_D, NUMBER_OF_TASKS_1_D);
	time_idle_task[4] = compute_time_idle_task(MATRIX_1_E, NUMBER_OF_TASKS_1_E);
	cost_of_plan[4] = compute_cost_scheduling_plan(MATRIX_1_E, NUMBER_OF_TASKS_1_E);
	time_idle_task[5] = compute_time_idle_task(MATRIX_1_PLAN_EMERGENCIA, NUMBER_OF_TASKS_PLAN_EMERGENCIA);
	cost_of_plan[5] = compute_cost_scheduling_plan(MATRIX_1_PLAN_EMERGENCIA, NUMBER_OF_TASKS_PLAN_EMERGENCIA);

    r1 = look_for_slot(SUNRISE_SLOT);
	r2 = look_for_slot(SUNSET_SLOT);
    x = rand() % (r2 - r1 + 1) + r1; // La produccion varia entre SUNRISE_SLOT y SUNSET_SLOT de otra manera no tiene efecto

    for (j = 0; j < MONTHS; j++)
    {
        ddMMyyhhmmss[0] = 1;
		ddMMyyhhmmss[1] = j + 1;
		ddMMyyhhmmss[2] = 2012;
		ddMMyyhhmmss[3] = 0;
		ddMMyyhhmmss[4] = 0;
		ddMMyyhhmmss[5] = 0;

		compute_efficiency(QoS, cost_of_plan, vector_efficiency, NUMBER_OF_PLANS_1);

		for (i = 0; i < NUMBER_OF_PLANS_1; i++)
			printf("Plan %d\tcost=%f\tQoS=%f\tefficiency=%f\n", i, cost_of_plan[i], QoS[i], vector_efficiency[i]);
		
		order_plans_by_efficiency(vector_efficiency, plans, NUMBER_OF_PLANS_1);

		printf("Order plans by efficiency\n");
		for (i = 0; i < NUMBER_OF_PLANS_1; i++)
			printf("Plan %d\tcost=%f\tQoS=%f\tefficiency=%f\n", plans[i], cost_of_plan[i], QoS[i], vector_efficiency[i]);

		get_time_from_seconds(0, ddMMyyhhmmss);
		printf("TIME:%d-%d-%d:%d:%d:%d\n", ddMMyyhhmmss[0], ddMMyyhhmmss[1], ddMMyyhhmmss[2], ddMMyyhhmmss[3], ddMMyyhhmmss[4], ddMMyyhhmmss[5]);

		hour = 0;

		for (i = 0; i < SLOTS; i++)
		{
			assignment[i] = plans[0];
		}

		compute_cost_assignment(plans[0], cost_of_plan, ddMMyyhhmmss, battery_at_slots, &hour, 1, j);

		optimal = 0;
		admisible = 0;
		n = NUMBER_OF_PLANS_1;
		
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
				upgrade(plans[0], assignment, battery_at_slots, cost_of_plan, QoS, j);
			}
			else
			{
				n = remove_plan_cost(plans[0], cost_of_plan, vector_efficiency, plans, n);
				printf("DOWNGRADE: Nuevo plan=%d con QoS=%f y efficiency=%f y cost=%f\n", plans[0], QoS[plans[0]], vector_efficiency[0], cost_of_plan[plans[0]]);
				printf("Numero de planes en el vector=%d\n", n);
				for (i = 0; i < n; i++)
					printf("Plan %d\tcost=%f\tQoS=%f\tefficiency=%f\n", i, cost_of_plan[i], QoS[i], vector_efficiency[i]);
				
				if (n == 0)
				{
					admisible = 1;
					break;
				}
				downgrade(plans[0], assignment, battery_at_slots, cost_of_plan, QoS, j);
			}
			
		}
		if (optimal == 1)
			printf("The solution is optimal\n");
		if (admisible == 1)
			printf("The solution is not admisible\n");

		compute_qos_assignment(assignment, QoS, j);
		sleep(3);

		compute_efficiency(QoS, cost_of_plan, vector_efficiency, NUMBER_OF_PLANS_1);
		order_plans_by_efficiency(vector_efficiency, plans, NUMBER_OF_PLANS_1);
		if (reoptimization(assignment, x, battery_at_slots, j, cost_of_plan, QoS, plans) == -1)
		{
			printf("The reoptimizations is not possible: Not admissible solution\n");
			sleep(1);
		}
    }
	return 0;
}

int compute_time_idle_task(int matrix[][TUPLA], int n)
{
	int time_idle_task = 0;
	int sum = 0;
	int i = 0;

	for (i = 0; i < n; i++)
		sum = sum + matrix[i][0];

	time_idle_task = matrix[0][1] - sum;
	return time_idle_task;
}
float compute_cost_scheduling_plan(int matrix[][TUPLA], int n)
{
	float cost = 0.0;
	float cost_idle_task = 0.0;
	float cost_other_tasks = 0.0;
	int i = 0;

	for (i = 0; i < n; i++)
	{
		printf("Tarea %d=(%d,%d,%d)\n", i, matrix[i][0], matrix[i][1], matrix[i][2]);
		cost_other_tasks += ((float)matrix[i][2] / (float)matrix[i][1]);
		cost_idle_task += ((float)matrix[i][0] / (float)matrix[i][1]);
	}
	cost = cost_other_tasks + ENERGY_IDLE_TASK * (1 - cost_idle_task);
	return cost;
}

void compute_efficiency(float *qos, float *cost_of_plan, float *vector_efficiency, int n)
{
	int i = 0;

	for (i = 0; i < n; i++)
		vector_efficiency[i] = (float)(qos[i] / cost_of_plan[i]);
}

void order_plans_by_efficiency(float *vector_efficiency, int *plans, int n)
{
	int i = 0, j = 0;
	int change;
	float aux;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n - i; j++)
		{
			if (vector_efficiency[j + 1] > vector_efficiency[j]) // Si el elemento j+1 es menor que el elemento j:
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

void get_time_from_seconds(int seconds, int *ddMMyyhhmmss)
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

double rectifier(double a, double b)
{
	double res = 0.0;
	res = a - b;
	if (res >= 0)
		return res;
	return 0.0;
}

int remove_plan_quality(int plan, float *vector_efficiency, int *plans, int n)
{
	int i = 0, j = 0;
	float vector_efficiency_aux[NUMBER_OF_PLANS_1];

	for (i = 0; i < n; i++)
	{
		if (QoS[plans[i]] > QoS[plan])
		{
			printf("asigno plan=%d con QoS=%f y efficiency=%f\n", i, QoS[plans[i]], vector_efficiency[i]);
			vector_efficiency_aux[j] = vector_efficiency[i];
			plans[j] = plans[i];
			j++;
		}
	}
	for (i = 0; i < j; i++)
		vector_efficiency[i] = vector_efficiency_aux[i];
	return j;
}

int remove_plan_cost(int plan, float *cost_of_plan, float *vector_efficiency, int *plans, int n)
{
	int i, j = 0;
	float vector_efficiency_aux[NUMBER_OF_PLANS_1];

	printf("El plan es=%d\tcoste del plan es=%f\n", plan, cost_of_plan[plan]);
	for (i = 0; i < n; i++)
	{
		if (cost_of_plan[i] < cost_of_plan[plan])
		{
			printf("asigno plan=%d con coste=%f y efficiency=%f\n", i, cost_of_plan[plans[i]], vector_efficiency[i]);
			vector_efficiency_aux[j] = vector_efficiency[i];
			plans[j] = i;
			j++;
		}
	}
	for (i = 0; i < j; i++)
		vector_efficiency[i] = vector_efficiency_aux[i];
	return j;
}
void look_for_time(int slot, int *ddMMyyhhmmss)
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

int look_for_slot(int time)
{
	int s;

	s = ceil(SLOTS / 24) * time;
	return s;
}

void compute_cost_assignment(int plan, float *cost_of_plan, int *ddMMyyhhmmss, double *battery_at_slots, int *hour, int init, int month)
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
	sprintf(filename, "%s-%d.csv", INITIAL, month);
	fs = fopen(filename, "w");

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
		fprintf(fs, "%d\t", i);
		fprintf(fs, "%d-%d-%d:%d:%d:%d\t", ddMMyyhhmmss[0], ddMMyyhhmmss[1], ddMMyyhhmmss[2], ddMMyyhhmmss[3], ddMMyyhhmmss[4], ddMMyyhhmmss[5]);
		fprintf(fs, "%d\t", plan);
		fprintf(fs, "%f\t", QoS[plan]);
		fprintf(fs, "%f\t", battery_at_slots[i]);
		fprintf(fs, "%f\t", consumption);
		fprintf(fs, "%f\n", ep_slot);
		// register data to display
		printf("SLOT=%d\tSCHEDULING PLAN=%d\tQoS[P]=%f\tB[%d]=%f\tconsumption=%f\testimation=%f\t", i, plan, QoS[plan], i, battery_at_slots[i], consumption, ep_slot);
		printf("TIME=%d-%d-%d:%d:%d:%d\n", ddMMyyhhmmss[0], ddMMyyhhmmss[1], ddMMyyhhmmss[2], ddMMyyhhmmss[3], ddMMyyhhmmss[4], ddMMyyhhmmss[5]);
		get_time_from_seconds(TIME_SLOTS, ddMMyyhhmmss);
	}
	fclose(fs);
}

void recompute_battery_level(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, int s, int hour, float *QoS, int month, int optimization)
{
	//Here we recompute the battery level at slots from s to SLOTS
	double battery_level, consumption = 0;
	int i;
	float ep_hour = 0.0;
	float ep_slot = 0.0;
	float ep_slot_remaining;
	float EP = 0.0;
	float K = D[month];
	float P = STD_HOURS[month] / K;
	char filename[256];
	FILE *fs;

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

	sprintf(filename, "%s-%d.csv", INITIAL, month);
	fs = fopen(filename, "a");
	// compute the production at this slot
	for (i = 1; i <= SLOTS; i++)
	{

		consumption = (cost_of_plan[assignment[i - 1]] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
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
		}
		fprintf(fs, "%d\t", i);
		fprintf(fs, "%d-%d-%d:%d:%d:%d\t", new_date[0], new_date[1], new_date[2], new_date[3], new_date[4], new_date[5]);
		fprintf(fs, "%d\t", assignment[i - 1]);
		fprintf(fs, "%f\t", QoS[assignment[i - 1]]);
		fprintf(fs, "%f\t", battery_at_slots[i]);
		fprintf(fs, "%f\t", consumption);
		fprintf(fs, "%f\n", ep_slot);
		get_time_from_seconds(TIME_SLOTS, new_date);
	}
	fclose(fs);
}

void upgrade(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, float *QoS, int month)
{
	int i = 0;
	int s;
	double consumption_new_plan = 0.0;
	double consumption_old_plan = 0.0;
	int old;

	// we compute the consumption in each slot for all the tasks: consumption(i)*duration_slot(i)*voltage
	s = look_for_slot(SUNRISE_SLOT);
	consumption_new_plan = (cost_of_plan[plan] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
	consumption_old_plan = (cost_of_plan[assignment[s - 1]] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
	while (((battery_at_slots[SLOTS] - battery_at_slots[0]) >= (consumption_new_plan - consumption_old_plan)) && (i < SLOTS))
	{
		old = assignment[s - 1];
		consumption_old_plan = (cost_of_plan[old] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
		assignment[s - 1] = plan;
		recompute_battery_level(plan, assignment, battery_at_slots, cost_of_plan, s, SUNRISE_SLOT, QoS, month, 0);
		printf("UPGRADE: due to change in the plan of slot=%d from %d to %d\n", s, old, plan);
		s = (s % SLOTS) + 1;
		i++;
	}
	printf("numero de veces que recorro el upgrade=%d desde %d a %d\n", i, look_for_slot(SUNRISE_SLOT), s);
	printf("UPGRADE: Battery_at_slot[SLOTS]=%f\tbattery_at_slots[0]=%f\tconsumption_old=%f\tconsumption_new=%f\n", battery_at_slots[SLOTS], battery_at_slots[0], consumption_old_plan, consumption_new_plan);
}

void downgrade(int plan, int *assignment, double *battery_at_slots, float *cost_of_plan, float *QoS, int month)
{
	int i = 0;
	int s;
	int old;
	s = look_for_slot(SUNSET_SLOT);
	while (((battery_at_slots[SLOTS] - battery_at_slots[0]) < 0) && (i < SLOTS))
	{
		old = assignment[s - 1];
		assignment[s - 1] = plan;
		recompute_battery_level(plan, assignment, battery_at_slots, cost_of_plan, s, SUNSET_SLOT, QoS, month, 0);
		printf("DOWNGRADE: due to change in the plan of slot=%d from %d to %d\n", s, old, plan);
		printf("DOWNGRADE: Battery_at_slot[SLOTS]=%f\n", battery_at_slots[SLOTS]);
		s = (s % SLOTS) + 1;
		i++;
	}
}

void compute_qos_assignment(int *assignment, float *QoS, int month)
{
	FILE *fq;
	int i = 0;
	float qos_assignment = 0.0;

	fq = fopen(MONTHLY_QOS, "a");
	for (i = 0; i < SLOTS; i++)
		qos_assignment += QoS[assignment[i]];

	printf("QoS of the assignment=%f\n", qos_assignment / SLOTS);
	fprintf(fq, "%d\t%f", month, qos_assignment / SLOTS);
	fclose(fq);
}
int reoptimization(int *assignment, int x, double *battery_at_slots, int month, float *cost_of_plan, float *QoS, int *plans)
{
	int i = 0, s, plan, plan_old, j, k;
	int replaced = 1;
	float consumption_new, consumption_old;
	int assignment_new[SLOTS];
	char filename[256];
	int ddMMyyhhmmss[6];

	FILE *fr;
	sprintf(filename, "%s-%d-%f.csv", REOPTIMIZATION, month, COEFFICIENT);
	fr = fopen(filename, "w");

	for (i = 0; i < SLOTS; i++)
		assignment_new[i] = assignment[i];

	look_for_time(x, ddMMyyhhmmss);
	ddMMyyhhmmss[1] = month + 1;
	//battery_at_slots[x]=battery_at_slots[x]+(battery_at_slots[x]*COEFFICIENT);
	recompute_battery_level(0, assignment, battery_at_slots, cost_of_plan, x + 1, ddMMyyhhmmss[3], QoS, month, 1);
	printf("after modifying the production at slot=%d battery_at_slots[SLOTS]=%f\t\n", x, battery_at_slots[SLOTS]);
	if (battery_at_slots[SLOTS] >= (battery_at_slots[0] + EPSILON))
	{
		replaced = 1;
		plan_old = assignment[x];
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
			printf("selecciono plan=%d\n", k);
			/*
			if (j>=NUMBER_OF_PLANS_1-1)
				return 0;
			*/
			while (s <= SLOTS)
			{
				plan_old = assignment[s - 1];
				consumption_old = (cost_of_plan[plan_old] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
				consumption_new = (cost_of_plan[k] / (60 * 60)) * Vld * TIME_SLOTS / (VOLTAGE * BATTERIES);
				printf("%d ", s);
				printf("%d-%d-%d:%d:%d:%d\t", ddMMyyhhmmss[0], ddMMyyhhmmss[1], ddMMyyhhmmss[2], ddMMyyhhmmss[3], ddMMyyhhmmss[4], ddMMyyhhmmss[5]);
				printf("Plan old=%d\t", plan_old);
				printf("QoS=%f\t", QoS[plan_old]);
				printf("consumption=%f\t", consumption_old);
				printf("B[s]=%f\t", battery_at_slots[s]);
				if ((k < NUMBER_OF_PLANS_1 - 1) && (k >= 0) && ((battery_at_slots[SLOTS] - battery_at_slots[0]) >= (consumption_new - consumption_old)))
				{
					assignment_new[s - 1] = k;
					recompute_battery_level(k, assignment_new, battery_at_slots, cost_of_plan, s, ddMMyyhhmmss[3], QoS, month, 1);
					printf("Plan NEW=%d\t", k);
					printf("consumption new=%f\t", consumption_new);
					printf("QoS=%f\t", QoS[k]);
					printf("B[i]=%f\t", battery_at_slots[s]);
					printf("B[k]=%f\n", battery_at_slots[SLOTS]);
					replaced = 1;
					get_time_from_seconds(TIME_SLOTS, ddMMyyhhmmss);
				}
				else
					printf("\n");
				s++;
			}
			j++;
			if (j <= NUMBER_OF_PLANS_1 - 1)
			{
				k = plans[j];
				printf("entro\n");
			}
			else
				replaced = 0;
		}
	}
	else
	{
		plan_old = assignment[x]; // because at the end of the slot x is when you modify the battery_level and in slot x+1 is the plan assignment[x]
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
			printf("selecciono plan=%d\n", k);
			sleep(1);
			s = x + 1;
			replaced = 0;
			while (s <= SLOTS)
			{
				printf("%d\t", s);
				printf("%d-%d-%d:%d:%d:%d\t", ddMMyyhhmmss[0], ddMMyyhhmmss[1], ddMMyyhhmmss[2], ddMMyyhhmmss[3], ddMMyyhhmmss[4], ddMMyyhhmmss[5]);
				printf("Plan old=%d\t", plan_old);
				printf("QoS=%f\t", QoS[plan_old]);
				printf("consumption=%f\t", consumption_old);
				printf("B[s]=%f\t", battery_at_slots[s]);
				printf("B[k]=%f\n", battery_at_slots[SLOTS]);
				if ((k >= 1) && (battery_at_slots[SLOTS] < battery_at_slots[0]))
				{
					assignment_new[s - 1] = k;
					recompute_battery_level(k, assignment_new, battery_at_slots, cost_of_plan, s, ddMMyyhhmmss[3], QoS, month, 1);
					printf("Plan NEW=%d\t", k);
					printf("QoS=%f\t", QoS[k]);
					printf("B[i]=%f\t", battery_at_slots[s]);
					printf("B[k]=%f\n", battery_at_slots[SLOTS]);
					get_time_from_seconds(TIME_SLOTS, ddMMyyhhmmss);
					replaced = 1;
				}
				else
					printf("\n");
				s++;
			}
			plan_old = k;
			j--;
			if (j < 0)
			{
				replaced = 0;
			}
			else
				k = plans[j];
		}
	}
	for (i = 1; i <= SLOTS; i++)
	{
		fprintf(fr, "%d\t", i);
		fprintf(fr, "%d\t", assignment[i - 1]);
		fprintf(fr, "%f\t", QoS[assignment[i - 1]]);
		fprintf(fr, "%d\t", assignment_new[i - 1]);
		fprintf(fr, "%f\t", QoS[assignment_new[i - 1]]);
		fprintf(fr, "%f\n", battery_at_slots[i]);
	}
	fclose(fr);
	for (i = 0; i < SLOTS; i++)
		assignment[i] = assignment_new[i];
	if (battery_at_slots[SLOTS] < battery_at_slots[0])
		return -1;
	return 0;
}