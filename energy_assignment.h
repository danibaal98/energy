#ifndef assignmentClass_h
#define assignmentClass_h

#define CAPACITY     2500
#define BATTERIES    2
#define VOLTAGE      1.5
#define EFFICIENCY   11.38 //energy efficiency of the solar cell
#define S            0.0036		 //surface of the solar cell
#define Vld          3.3			 //specific voltage of load
#define H            12			 //D(t) =(t - h)2/4p+k
#define MONTHS       12		 //we start the execution 1 of January of 2012, at 8:00 am
#define TIME         0 //8:00 am. (hours)
#define SUNRISE_SLOT 6
#define SUNSET_SLOT  18
#define EPSILON      0.0001 // epsilon=10^-4?
#define COEFFICIENT  (-0.01)

// definition of the applications
#define NUMBER_OF_PLANS_1   6 //mas uno porque incluyo el plan de emergencia
#define NUMBER_OF_TASKS_1_A 3
#define NUMBER_OF_TASKS_1_B 4
#define NUMBER_OF_TASKS_1_C 1
#define NUMBER_OF_TASKS_1_D 2
#define NUMBER_OF_TASKS_1_E 1

#define NUMBER_OF_TASKS_PLAN_EMERGENCIA 1

#define ENERGY_IDLE_TASK 2
#define TUPLA            3

#define TIME_SLOTS 288 //in seconds
#define SLOTS (3600 * 24 / TIME_SLOTS)

class assignmentClass
{
 public:
    assignmentClass(void);

    int* assign_plan(int month);
    int compute_time_idle_task(int matrix[][TUPLA], int n);
    float compute_cost_scheduling_plan(int matrix[][TUPLA], int n);
    void compute_efficiency(float *qos, float *cost_of_plan, float *vector_efficiency, int n);
    void get_time_from_seconds(int seconds, int *ddMMyyhhmmss);
    void order_plans_by_efficiency(float *vector_efficiency, int *plans, int n);
    int remove_plan_quality(int plan, float *vector_efficiency, int *plans, int n);
    int remove_plan_cost(int plan, float *cost_of_plan, float *vector_efficiency, int *plans, int n);
    double rectifier(double a, double b);
    void upgrade(int plan, int *assignments, double *battery_at_slots, float *cost_of_plan, float *QoS, int month);
    void downgrade(int plan, int *assignments, double *battery_at_slots, float *cost_of_plan, float *QoS, int month);
    int look_for_slot(int time);
    void look_for_time(int slot, int *ddMMyyhhmmss);
    void compute_cost_assignment(int plan, float *cost_of_plan, int *ddMMyyhhmmss, double *battery_at_slots, int *hour, int init, int month);
    void recompute_battery_level(int plan, int *assignments, double *battery_at_slots, float *cost_of_plan, int s, int hour, float *QoS, int month, int optimization);
    void compute_qos_assignment(int *assignments, float *QoS, int month);
    int reoptimization(int *assignments, int x, double *battery_at_slots, int month, float *cost_of_plan, float *QoS, int *plans);

};
extern assignmentClass assignment;

#endif
