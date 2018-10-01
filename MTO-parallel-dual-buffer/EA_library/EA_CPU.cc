#include "EA_CPU.h"
EA_CPU::EA_CPU()
{

}

EA_CPU::~EA_CPU()
{

}
int EA_CPU::Initilize(IslandInfo island_info, ProblemInfo problem_info, NodeInfo node_info)
{
    problem_info_ = problem_info;
    island_info_ = island_info;
    node_info_ = node_info;
    for (int i = 0; i < problem_info_.task_num; i++)
    {
        CEC2014 tmp_cec2014;
        int task_ID = node_info_.task_IDs[i];
        tmp_cec2014.Initilize(problem_info_.task_list[task_ID].function_ID, problem_info_.dim, problem_info_.task_list[task_ID].option, problem_info_.task_list[task_ID].seed);
        cec2014_.push_back(tmp_cec2014);
    }
    srand(problem_info_.seed);
    return 0;
}

int EA_CPU::Unitilize()
{
    for (int i = 0; i < problem_info_.task_list.size(); i++)
        cec2014_[i].Unitilize();
}

int EA_CPU::InitilizePopulation(Population & population)
{   
    int task_ID = node_info_.task_IDs[node_info_.node_ID];
    for(int i = 0; i < island_info_.island_size; i++)
    {
        Individual tmp_individual;

        for (int j = 0; j < problem_info_.dim; j++)
            tmp_individual.elements.push_back(random_.RandRealUnif(problem_info_.min_bound, problem_info_.max_bound));

        tmp_individual.fitness_value = cec2014_[task_ID].EvaluateFitness(tmp_individual.elements);

        population.push_back(tmp_individual);
    }

    return 0;
}


real EA_CPU::CheckBound(real to_check_elements, real min_bound, real max_bound)
{
    while ((to_check_elements < min_bound) || (to_check_elements > max_bound))
    {
        if (to_check_elements < min_bound)
            to_check_elements = min_bound + (min_bound - to_check_elements);
        if (to_check_elements > max_bound)
            to_check_elements = max_bound - (to_check_elements - max_bound);
    }
    return to_check_elements;
}

Individual EA_CPU::FindBestIndividual(Population & population)
{
    int best_individual_ind = 0;
    double best_individual_fitness_value = population[0].fitness_value;
    for(int i = 1; i < island_info_.island_size; i++)
    {
        if(population[i].fitness_value < best_individual_fitness_value)
        {
            best_individual_ind = i;
            best_individual_fitness_value = population[i].fitness_value;
        }
    }
    return population[best_individual_ind];
}


real EA_CPU::EvaluateFitness(Individual & individual, int task_ID)
{
    return cec2014_[task_ID].EvaluateFitness(individual.elements);
}

#ifdef DEBUG
void EA_CPU::PrintTaskDetails(string file_name)
{
    ifstream exist_file;
    ofstream file;

        file.open(file_name.c_str(),ios::app);
        file<<"--------------------Problem ID = "<<problem_info_.problem_ID <<"--------run ID = "<<problem_info_.run_ID<<"------------------"<<endl;
        file<< "function_IDs";
        for(int i = 0; i < problem_info_.task_num; i++)
            file<<'\t'<<problem_info_.task_list[i].function_ID;
        file<<endl;
      
        file<< "correlation";
        for(int i = 0; i < problem_info_.task_num; i++)
            file<<'\t'<<cec2014_[i].Correlation();
        file<<endl;

        for(int i = 0; i < problem_info_.task_num; i++)
        {
            file<< "gbest for task "<< i;

            for(int j = 0; j < problem_info_.dim; j++)
                file<<'\t'<<cec2014_[i].Shift(j);
            file<<endl;
        }

        file<< "task_IDs";
        for(int i = 0; i < island_info_.island_num; i++)
            file<<'\t'<<node_info_.task_IDs[i];
        file<<endl;
        file.close();
}
#endif
















