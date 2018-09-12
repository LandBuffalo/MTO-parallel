#include "EA_CPU.h"

DE_CPU::DE_CPU()
{

}

DE_CPU::~DE_CPU()
{
    EA_CPU::Unitilize();
}

string DE_CPU::GetParameters()
{
	string str;
	ostringstream temp1, temp2;
	string parameters = "CR/F=";

	double CR = DE_info_.CR;
	temp1<<CR;
	str=temp1.str();
	parameters.append(str);

	parameters.append("/");
	double F = DE_info_.F;
	temp2<<F;
	str=temp2.str();
	parameters.append(str);

	if(DE_info_.strategy_ID == 0)
		parameters.append("_current/1/bin");
	else if(DE_info_.strategy_ID == 1)
		parameters.append("_current/2/bin");
	else if(DE_info_.strategy_ID == 2)
		parameters.append("_current-to-best/1/bin");
	else if(DE_info_.strategy_ID == 3)
		parameters.append("_current-to-best/2/bin");
	else if(DE_info_.strategy_ID == 4)
		parameters.append("_rand/1/bin");
	else if(DE_info_.strategy_ID == 5)
		parameters.append("_rand/2/bin");
	else if(DE_info_.strategy_ID == 6)
		parameters.append("_best/1/bin");
	else if(DE_info_.strategy_ID == 7)
		parameters.append("_best/2/bin");
	else if(DE_info_.strategy_ID == 8)
		parameters.append("_current_to_rand/1/bin");
	return parameters;
}

int DE_CPU::Initilize(IslandInfo island_info, ProblemInfo problem_info, NodeInfo node_info)
{
    EA_CPU::Initilize(island_info, problem_info, node_info);

    DE_info_.CR = 0.1;
    DE_info_.F = 0.5;
    DE_info_.strategy_ID = 4;

    return 0;
}

int DE_CPU::InitilizePopulation(Population & population)
{
    EA_CPU::InitilizePopulation(population);
    candidate_ = population;

    return 0;
}

int DE_CPU::Unitilize()
{
    return 0;
}

int DE_CPU::Reproduce(Population & population)
{
    Individual best_individual = FindBestIndividual(population);
    vector<int> r = random_.Permutate(island_info_.island_size, 5);

    double F = DE_info_.F;
    double CR = DE_info_.CR;

    for (int i = 0; i < island_info_.island_size; i++)
    {
        for (int j = 0; j < problem_info_.dim; j++)
        {
            switch (DE_info_.strategy_ID)
            {
                case 0:
                    candidate_[i].elements[j] = population[i].elements[j] + F * (population[r[0]].elements[j] - population[r[1]].elements[j]);
                    break;
                case 1:
                    candidate_[i].elements[j] = population[i].elements[j] + F * (population[r[0]].elements[j] - population[r[1]].elements[j]) + \
                    + F * (population[r[2]].elements[j] - population[r[3]].elements[j]);
                    break;
                case 2:
                    candidate_[i].elements[j] = population[i].elements[j] + F * (best_individual.elements[j] - population[i].elements[j]) + \
                    + F * (population[r[0]].elements[j] - population[r[1]].elements[j]);
                    break;
                case 3:
                    candidate_[i].elements[j] = population[i].elements[j] + F * (best_individual.elements[j] - population[i].elements[j]) + \
                    + F * (population[r[0]].elements[j] - population[r[1]].elements[j]) + F * (population[r[2]].elements[j] - population[r[3]].elements[j]);
                    break;
                case 4:
                    candidate_[i].elements[j] = population[r[0]].elements[j] + F * (population[r[1]].elements[j] - population[r[2]].elements[j]);
                    break;
                case 5:
                    candidate_[i].elements[j] = population[r[0]].elements[j] + F * (population[r[1]].elements[j] - population[r[2]].elements[j]) + \
                    + F * (population[r[3]].elements[j] - population[r[4]].elements[j]);
                    break;
                case 6:
                    candidate_[i].elements[j] = best_individual.elements[j] + F * (population[r[0]].elements[j] - population[r[1]].elements[j]);
                    break;
                case 7:
                    candidate_[i].elements[j] = best_individual.elements[j] + F * (population[r[0]].elements[j] - population[r[1]].elements[j]) + \
                    + F * (population[r[2]].elements[j] - population[r[3]].elements[j]);
                    break;
                case 8:
                    candidate_[i].elements[j] = population[i].elements[j] + F * (population[r[0]].elements[j] - population[i].elements[j]) + \
                    + F * (population[r[1]].elements[j] - population[r[2]].elements[j]) + F * (population[r[3]].elements[j] - population[r[4]].elements[j]);
                    break;
                default:
                    break;
            }
            if (random_.RandRealUnif(0, 1) > CR && j != random_.RandIntUnif(0, problem_info_.dim - 1))
                candidate_[i].elements[j] = population[i].elements[j];
            candidate_[i].elements[j] = CheckBound(candidate_[i].elements[j], problem_info_.min_bound, problem_info_.max_bound);
        }
        int task_ID = node_info_.task_IDs[node_info_.node_ID];
        candidate_[i].fitness_value = EvaluateFitness(candidate_[i], task_ID);
    }
    return 0;
}

int DE_CPU::SelectSurvival(Population & population)
{
    for (int i = 0; i < island_info_.island_size; i++)
    {
        if (candidate_[i].fitness_value < population[i].fitness_value)
        {
            population[i].fitness_value = candidate_[i].fitness_value;
            population[i].elements = candidate_[i].elements;
        }
    }
    return 0;
}

int DE_CPU::Run(Population & population)
{
    Reproduce(population);
    SelectSurvival(population);
    return 0;
}
