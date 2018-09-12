#ifndef __CPU_EA_LIBRARY_H__
#define __CPU_EA_LIBRARY_H__
#include <stdio.h>
#include "config.h"
#include "CEC2014.h"
#include "random.h"
#include <sstream>
typedef double real;

struct DEInfo
{
	double CR;
	double F;
	int strategy_ID;
};

class EA_CPU
{
protected:
    ProblemInfo             problem_info_;
    NodeInfo				node_info_;
    IslandInfo              island_info_;
    vector<CEC2014>   		cec2014_;
    Random          		random_;
public:
                    		EA_CPU();
                    		~EA_CPU();
    virtual int             InitilizePopulation(Population & population);
	virtual int     		Initilize(IslandInfo island_info, ProblemInfo problem_info, NodeInfo node_info);
    virtual int             Unitilize();
    real 					EvaluateFitness(Individual & individual, int task_ID);

	Individual     	        FindBestIndividual(Population & population);
    virtual string          GetParameters() = 0;
	virtual int     		Run(Population & population) = 0;

    real                    CheckBound(real to_check_elements, real min_bound, real max_bound);
#ifdef DEBUG
    void                    PrintTaskDetails(string file_name);
#endif
};

class DE_CPU : public EA_CPU
{
private:
	DEInfo          		DE_info_;
    Population              candidate_;
    int                     Reproduce(Population & population);
    int                     SelectSurvival(Population & population);
    virtual int             InitilizePopulation(Population & population);

public:
							DE_CPU();
							~DE_CPU();
	virtual int            	Initilize(IslandInfo island_info, ProblemInfo problem_info, NodeInfo node_info);
	virtual int             Unitilize();
	virtual int             Run(Population & population);
	virtual string          GetParameters();
};


#endif
