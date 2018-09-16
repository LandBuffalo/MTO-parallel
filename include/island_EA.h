#ifndef __ISLANDEA_H__
#define __ISLANDEA_H__
#pragma once
#include "config.h"
#include "random.h"
#include "EA_CPU.h"
#include "migrate.h"

class IslandEA
{
private:
	EA_CPU	*				EA_CPU_;
	Random                  random_;
	NodeInfo				node_info_;
	Migrate 				migrate_;

	ProblemInfo				problem_info_;
	IslandInfo				island_info_;

	Individual 				best_individuals_;
    Population 				sub_population_;

	DisplayUnit		 		display_unit_;
    string 					file_name_;
    string 					debug_file_name_;

	int						RunEA(DisplayUnit display_unit, int &flag_converge);
    DisplayUnit             RecordDisplayUnit(real current_time, real communication_time, int current_FEs, int total_insert, int total_success_insert);
	int 					RecvResultFromOtherIsland(vector<DisplayUnit> &total_display_unit);
	int 					MergeResults(vector<DisplayUnit> &total_display_unit);
	int 					CheckAndCreatRecordFile();
	int 					SendResultToIsland0();
    int                    	Finish();
    int         			RegroupIsland();
    int 					CalIslandNumCurrentTask();
public:
							IslandEA();
							~IslandEA();
	int 					Initilize(IslandInfo island_info, ProblemInfo problem_info, NodeInfo node_info);
	int 					Unitilize();
	int						Execute();
};

#endif
