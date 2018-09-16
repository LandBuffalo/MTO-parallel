#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <cmath>
#include <time.h>
#include <numeric>
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
using namespace std;

#define TOTAL_RECORD_NUM		20
#define EMIGRATIONS_ISLAND     	0
#define EMIGRATIONS_EA     		1
#define FLAG_FINISH       		2
#define FLAG_DISPLAY_UNIT       3

//#define COMPUTING_TIME
#define DEBUG
//#define NO_INTERACTION
//#define DISPLAY
//#define DIVERSITY
typedef double real;
struct Individual
{
	vector<double> elements;
	double fitness_value;
};

struct Task
{
	int function_ID;
	int option;
	int seed;
};

typedef vector<Individual> Population;

struct ProblemInfo
{
	int dim;
	int run_ID;
	int problem_ID;
	int max_base_FEs;
	int seed;
	int computing_time;
	real max_bound;
	real min_bound;
	int task_num;
	vector<Task> task_list;
};


struct NodeInfo
{
	int node_ID;
	int node_num;
	int GPU_num;
	int GPU_ID;
	vector<int> task_IDs;
};

struct IslandInfo
{
	int island_size;
	int island_num;
	int interval;
	int migration_size;
    int buffer_capacity;
	real migration_rate;
  	int subisland_num;
	int interval_tmp;
    string configure_EA;
    string regroup_option;
    string migration_topology;
	string buffer_manage;
	string replace_policy;

};

struct DisplayUnit
{
	real FEs;
	real time;
	real communication_percentage;
	real fitness_value;
	real total_insert;
	real total_success_insert;
};

#endif
