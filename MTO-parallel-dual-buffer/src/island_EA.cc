#include "island_EA.h"
IslandEA::IslandEA()
{
}

IslandEA::~IslandEA()
{

}

int IslandEA::CheckAndCreatRecordFile()
{
#ifndef	NO_INTERACTION
    file_name_ = "./Results/MTO_parallel.csv";
#else
    file_name_ = "./Results/MTO_parallel_wo_interaction.csv";
#endif
    ifstream exist_file;
    ofstream file;
    exist_file.open(file_name_.c_str());

    if(!exist_file)
    {
        file.open(file_name_.c_str());
        file<< "problem_ID,task_num,run_ID,min_time,avg_time,max_time,total_FEs,island_num,pop_size_per_task";
        for(int i = 0; i < problem_info_.task_num; i++)
            file<<','<<"fitness_value_"<<i + 1;
        for(int i = 0; i < problem_info_.task_num; i++)
            file<<','<<"time_task_"<<i + 1;
        for(int i = 0; i < problem_info_.task_num; i++)
            file<<','<<"FEs"<<i + 1;
        for(int i = 0; i < problem_info_.task_num; i++)
            file<<','<<"comm_pecentage"<<i + 1;
        for(int i = 0; i < problem_info_.task_num; i++)
            file<<','<<"total_insert"<<i + 1;
        for(int i = 0; i < problem_info_.task_num; i++)
            file<<','<<"total_success_insert"<<i + 1;
        file<<endl;
        file.close();
    }
    else
        exist_file.close();

#ifdef DEBUG
    if (node_info_.node_ID == problem_info_.problem_ID)
    {
	    stringstream tmp_task_num;
	    string s_task_num;
	    tmp_task_num << problem_info_.task_num;
	    tmp_task_num >> s_task_num;
	    debug_file_name_ = "./Results/Debug/problems_task_num=" + s_task_num + ".csv";
	    EA_CPU_->PrintTaskDetails(debug_file_name_);
    }
#endif

    return 0;
}


int IslandEA::Initilize(IslandInfo island_info, ProblemInfo problem_info, NodeInfo node_info)
{
    problem_info_ = problem_info;
    island_info_ = island_info;

    node_info_ = node_info;
    EA_CPU_ = new DE_CPU();

    EA_CPU_->Initilize(island_info_, problem_info_, node_info_);
    CheckAndCreatRecordFile();
    srand(problem_info_.seed);
    EA_CPU_->InitilizePopulation(sub_population_);
    migrate_.Initilize(island_info_, problem_info_, node_info_);
    return 0;

}

int IslandEA::Unitilize()
{
    sub_population_.clear();
    EA_CPU_->Unitilize();
    delete EA_CPU_;
    migrate_.Unitilize();

    return 0;
}

int IslandEA::Finish()
{
    migrate_.Finish();
    MPI_Barrier(MPI_COMM_WORLD);

    if(node_info_.node_ID != 0)
    {
        SendResultToIsland0();
    }
    else
    {
        vector<DisplayUnit> total_display_unit;
        RecvResultFromOtherIsland(total_display_unit);
        MergeResults(total_display_unit);
    }
    return 0;
}

int IslandEA::RunEA(DisplayUnit display_unit, int &flag_converge)
{
    EA_CPU_->Run(sub_population_);
    if(display_unit.fitness_value < 1e-8 && flag_converge == 0)
    {
	    display_unit_ = display_unit;
	    flag_converge = 1;
    }
    return 0;
}

int IslandEA::CalIslandNumCurrentTask()
{
    int current_task_ID = node_info_.task_IDs[node_info_.node_ID];
    int count = 0;
    for(int i = 0; i < node_info_.node_num; i++)
    {
        if(current_task_ID == node_info_.task_IDs[i])
            count++;
    }
    return count;
}
int IslandEA::Execute()
{
    int generation = 0;
    int current_FEs = island_info_.island_size;
    double start_time = MPI_Wtime();
    real communication_time = 0;
    int flag_converge = 0;
    int task_count = CalIslandNumCurrentTask();
    long int total_FEs = problem_info_.max_base_FEs * problem_info_.dim / (task_count + 0.0);
    DisplayUnit display_unit;

#ifndef COMPUTING_TIME
    while(current_FEs < total_FEs)
#else
    while(MPI_Wtime() - start_time < problem_info_.computing_time)
#endif   
    {
        display_unit = RecordDisplayUnit((real) (MPI_Wtime() - start_time), communication_time, current_FEs, migrate_.TotalInsert(), migrate_.TotalSuccessInsert());
        RunEA(display_unit, flag_converge);
        if (island_info_.interval > 1)
        {
            if ( generation % island_info_.interval == 0)
            {
                double tmp_time = MPI_Wtime();
               	int extra_FEs = migrate_.MigrateIn(sub_population_, EA_CPU_);

                communication_time += (real) (MPI_Wtime() - tmp_time);
                current_FEs += extra_FEs;
            }
            double tmp_time = MPI_Wtime();
            migrate_.MigrateOut(sub_population_);
            communication_time += (real) (MPI_Wtime() - tmp_time);
        }
        generation++;
        current_FEs += island_info_.island_size;
    }
    if(flag_converge == 0)
        display_unit_ = RecordDisplayUnit((real) (MPI_Wtime() - start_time), communication_time, current_FEs, migrate_.TotalInsert(), migrate_.TotalSuccessInsert());

    Finish();
    return 0;

}

DisplayUnit IslandEA::RecordDisplayUnit(real current_time,  real communication_time, int current_FEs, int total_insert, int total_success_insert)
{
    DisplayUnit display_unit;
	display_unit.time = current_time;
	display_unit.FEs = current_FEs;
    display_unit.fitness_value = EA_CPU_->FindBestIndividual(sub_population_).fitness_value;
	display_unit.communication_percentage = communication_time / current_time;
    display_unit.total_success_insert = total_success_insert;
    display_unit.total_insert = total_insert;

    return display_unit;
}

int IslandEA::MergeResults(vector<DisplayUnit> &total_display_unit)
{
    vector<real> fitness_values(problem_info_.task_num, 1e20);
    vector<real> times_tasks(problem_info_.task_num, 1e20);
    vector<real> FEs_tasks(problem_info_.task_num, 0);
    vector<real> communication_percentage(problem_info_.task_num, 0);
    vector<real> total_insert(problem_info_.task_num, 0);
    vector<real> total_success_insert(problem_info_.task_num, 0);

    real min_time = 1e20, avg_time = 0, max_time = 0;
    total_display_unit[0] = display_unit_;
    //printf("%d\t%d\n", node_info_.node_ID, node_info_.node_num);
    for(int i = 0; i < node_info_.node_num; i++)
    {
    	int task_ID = node_info_.task_IDs[i];
        if(fitness_values[task_ID] > total_display_unit[i].fitness_value)
        {
            fitness_values[task_ID] = total_display_unit[i].fitness_value;
            times_tasks[task_ID] = total_display_unit[i].time;
            FEs_tasks[task_ID] = total_display_unit[i].FEs;
            communication_percentage[task_ID] = total_display_unit[i].communication_percentage;
        }

        if(max_time < total_display_unit[i].time)
            max_time = total_display_unit[i].time;
        if(min_time > total_display_unit[i].time)
            min_time = total_display_unit[i].time;
        avg_time += total_display_unit[i].time;
        total_insert[task_ID] += total_display_unit[i].total_insert;
        total_success_insert[task_ID] += total_display_unit[i].total_success_insert;
    }
    avg_time = avg_time / node_info_.node_num;
    ofstream file;
    file.open(file_name_.c_str(), ios::app);

    int run_ID = problem_info_.run_ID;
    int dim = problem_info_.dim;
    long int total_FEs = problem_info_.max_base_FEs * problem_info_.dim;
    int pop_size_per_task = island_info_.island_num * island_info_.island_size / problem_info_.task_num;

#ifdef COMPUTING_TIME
    file<<problem_info_.problem_ID<<','<<problem_info_.task_num<<','<<run_ID<<','<<problem_info_.computing_time<<','<<problem_info_.computing_time<<','<<problem_info_.computing_time <<','<<total_FEs<<','<<island_info_.island_num<<','<<pop_size_per_task; 
#else
    file<<problem_info_.problem_ID<<','<<problem_info_.task_num<<','<<run_ID<<','<<min_time<<','<<avg_time<<','<< max_time<<','<<total_FEs<<','<<island_info_.island_num<<','<<pop_size_per_task; 
#endif
    for (int i = 0; i < problem_info_.task_num; i++)
        file<<','<<fitness_values[i];
    
    for (int i = 0; i < problem_info_.task_num; i++)
    	file<<','<<times_tasks[i];

    for (int i = 0; i < problem_info_.task_num; i++)
        file<<','<<FEs_tasks[i];

    for (int i = 0; i < problem_info_.task_num; i++)
        file<<','<<communication_percentage[i];

    for (int i = 0; i < problem_info_.task_num; i++)
        file<<','<<total_insert[i];

    for (int i = 0; i < problem_info_.task_num; i++)
        file<<','<<total_success_insert[i];
    file<<endl;
    file.close();
    return 0;

}


int IslandEA::SendResultToIsland0()
{
    real *msg = new real[6];
    msg[0] = display_unit_.time;
    msg[1] = display_unit_.FEs;
    msg[2] = display_unit_.communication_percentage;
    msg[3] = display_unit_.fitness_value;
    msg[4] = display_unit_.total_insert;
    msg[5] = display_unit_.total_success_insert;

    int tag = 10 * problem_info_.run_ID + FLAG_DISPLAY_UNIT;
    MPI_Send(msg, 6, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);

    delete [] msg;

    return 0;
}

int IslandEA::RecvResultFromOtherIsland(vector<DisplayUnit> &total_display_unit)
{
    MPI_Status mpi_status;
    DisplayUnit tmp_display_unit;

    for(int i = 0; i < node_info_.node_num; i++)
        total_display_unit.push_back(tmp_display_unit);

    real *msg = new real[6];
    for(int i = 1; i < node_info_.node_num; i++)
    {
        int tag = 10 * problem_info_.run_ID + FLAG_DISPLAY_UNIT;
        MPI_Recv(msg, 6, MPI_DOUBLE, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &mpi_status);

        DisplayUnit tmp_display_unit;
        tmp_display_unit.time = msg[0];
        tmp_display_unit.FEs = msg[1];
        tmp_display_unit.communication_percentage = msg[2];
        tmp_display_unit.fitness_value = msg[3];
        tmp_display_unit.total_insert = msg[4];
        tmp_display_unit.total_success_insert = msg[5];

        total_display_unit[mpi_status.MPI_SOURCE] = tmp_display_unit;
    }
    delete []msg;
    return 0;
}