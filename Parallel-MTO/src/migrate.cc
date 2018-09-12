#include "migrate.h"

Migrate::Migrate()
{

}

Migrate::~Migrate()
{

}

int Migrate::Initilize(IslandInfo island_info, ProblemInfo problem_info, NodeInfo node_info)
{
    island_info_ = island_info;
    problem_info_ = problem_info;
    node_info_ = node_info;
    total_insert_ = 0;
    total_success_insert_ = 0;

    int message_length = island_info_.migration_size * (problem_info_.dim + 1);
    send_msg_to_other_EA_ = new real[message_length];
    buffer_manage_ = new DiversityPreserving(island_info.buffer_capacity);
    success_sent_flag_ = 1;
    return 0;
}

int Migrate::Unitilize()
{
    delete []send_msg_to_other_EA_;
    delete buffer_manage_;
    destinations_.clear();

    return 0;
}
double Migrate::SuccessRate()
{
    return total_success_insert_ / (total_insert_ + 0.0);
}
int Migrate::TotalSuccessInsert()
{
    return total_success_insert_;
}
int Migrate::TotalInsert()
{
    return total_insert_;
}

int Migrate::UpdateDestination()
{
   if(destinations_.size() == 0)
    {
        if(island_info_.migration_topology == "dynamicConnect")
        {
            for (int i = 0; i < node_info_.node_num; i++)
                if( random_.RandRealUnif(0,1) < island_info_.migration_rate && i != node_info_.node_ID)
                    destinations_.push_back(i);
        }
        if(island_info_.migration_topology == "undirectRing")
        {
            if(node_info_.node_ID < node_info_.node_num - 1)
                destinations_.push_back(node_info_.node_ID + 1);

        }
        if(island_info_.migration_topology == "ring")
        {
            destinations_.push_back((node_info_.node_num + node_info_.node_ID - 1) % node_info_.node_num);
            destinations_.push_back((node_info_.node_num + node_info_.node_ID + 1) % node_info_.node_num);
        }
        if(island_info_.migration_topology == "lattice")
        {
            int row = sqrt(node_info_.node_num);
            int i = node_info_.node_ID;
            if(i / row == 0)
            {
                if(i % row == 0)
                {
                    destinations_.push_back(i + 1);
                    destinations_.push_back(i + row);
                }
                else if(i % row == (row - 1))
                {
                    destinations_.push_back(i - 1);
                    destinations_.push_back(i + row);
                }
                else
                {
                    destinations_.push_back(i - 1);
                    destinations_.push_back(i + 1);
                    destinations_.push_back(i + row);
                }
            }
            else if(i / row == (row - 1))
            {
                if(i % row == 0)
                {
                    destinations_.push_back(i + 1);
                    destinations_.push_back(i - row);
                }
                else if(i % row == (row - 1))
                {
                    destinations_.push_back(i - 1);
                    destinations_.push_back(i - row);
                }
                else
                {
                    destinations_.push_back(i - 1);
                    destinations_.push_back(i + 1);
                    destinations_.push_back(i - row);
                }
            }
            else
            {
                if(i % row == 0)
                {
                    destinations_.push_back(i + 1);
                    destinations_.push_back(i + row);
                    destinations_.push_back(i - row);
                }
                else if(i % row == (row - 1))
                {
                    destinations_.push_back(i - 1);
                    destinations_.push_back(i + row);
                    destinations_.push_back(i - row);
                }
                else
                {
                    destinations_.push_back(i - 1);
                    destinations_.push_back(i + 1);
                    destinations_.push_back(i + row);
                    destinations_.push_back(i - row);
                }
            }
        }
    }
    return 0;
}
int Migrate::UpdatePopulation(Population & population)
{
    int count = 0;
    Population emigration_import;
    buffer_manage_->SelectFromBuffer(emigration_import, island_info_.migration_size);
    total_insert_ += emigration_import.size();

    int worst_fitness_value_ID = 0;
    
    if (island_info_.replace_policy == "worst")
    {
        for (int i = 0; i < emigration_import.size(); i++)
        {
            int worst_ID = 0;
            real worst_value = 0;
            
            for (int j = 0; j < population.size(); j++)
            {
                if(worst_value < population[j].fitness_value)
                {
                    worst_value = population[j].fitness_value;
                    worst_ID = j;
                }
            }
            if(worst_value > emigration_import[i].fitness_value)
            {
                population[worst_ID] = emigration_import[i]; 
                total_success_insert_++;
                count++;
            }
        }
    }
    else if (island_info_.replace_policy == "tournament")
    {
        vector<int> random_IDs = random_.Permutate(population.size(), emigration_import.size());
        for (int i = 0; i < emigration_import.size(); i++)
        {
            if(population[random_IDs[i]].fitness_value > emigration_import[i].fitness_value)
            {
                population[random_IDs[i]] = emigration_import[i]; 
                total_success_insert_++;
                count++;
            }
        }
        
    }
    
    return count;
}
int Migrate::UpdatePopulationBuffer(Population &population, EA_CPU *EA_CPU)
{

    int task_num = problem_info_.task_list.size();
    v_original_fitness_values_.clear();
    population_buffer_.clear();
    population_buffer_ = population;
    for(int i = 0; i < task_num; i++)
    {
        vector<real> original_fitness_values;
        for(int j = 0; j < population.size(); j++)
            original_fitness_values.push_back(EA_CPU->EvaluateFitness(population[j], i));

        v_original_fitness_values_.push_back(original_fitness_values);
     }
    return 0;

}
int Migrate::CheckAndRecvEmigrations()
{
    MPI_Status mpi_status;
    int flag = 0;
    int tag = 10 * problem_info_.run_ID + EMIGRATIONS_ISLAND;
    MPI_Iprobe(MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &flag, &mpi_status);
    if(flag == 1)
    {
        int count = 0;
        MPI_Get_count(&mpi_status, MPI_DOUBLE, &count);

        int message_length = count;
        real * msg_recv = new real[message_length];
        int source = mpi_status.MPI_SOURCE;

        MPI_Recv(msg_recv, message_length, MPI_DOUBLE, source, tag, MPI_COMM_WORLD, &mpi_status);

        Population emigration_import;
        DeserialMsgToIndividual(emigration_import, msg_recv, count / (problem_info_.dim + 1));

        buffer_manage_->UpdateBuffer(emigration_import);
        delete [] msg_recv;
    }

    return 0;
}

int Migrate::SendEmigrations()
{
    MPI_Status mpi_status;

    if(success_sent_flag_ == 0)
    {
        MPI_Test(&mpi_request_, &success_sent_flag_, &mpi_status);
    }
    if (success_sent_flag_ == 1 && destinations_.size() > 0)
    {
        Population emigration_export;
        int task_ID = node_info_.task_IDs[destinations_[0]];
        int current_task_ID = node_info_.task_IDs[node_info_.node_ID];
#ifdef NO_INTERACTION        
        if (task_ID != current_task_ID)
            return 0;
#endif
        vector<real> sorted_fitness_values;
        sorted_fitness_values = v_original_fitness_values_[task_ID];
        sort(sorted_fitness_values.begin(), sorted_fitness_values.begin() + population_buffer_.size());

        for(int i = 0; i < island_info_.migration_size; i++)
        {
            int index = distance(v_original_fitness_values_[task_ID].begin(), find(v_original_fitness_values_[task_ID].begin(), v_original_fitness_values_[task_ID].end(), sorted_fitness_values[i]));
            Individual selected_individual = population_buffer_[index];
            selected_individual.fitness_value = sorted_fitness_values[i];
            emigration_export.push_back(selected_individual);
        }
        int message_length = island_info_.migration_size * (problem_info_.dim + 1);
        SerialIndividualToMsg(send_msg_to_other_EA_, emigration_export);
        int tag = 10 * problem_info_.run_ID + EMIGRATIONS_ISLAND;
        MPI_Isend(send_msg_to_other_EA_, message_length, MPI_DOUBLE, destinations_[0], tag, MPI_COMM_WORLD, &mpi_request_);
        destinations_.erase(destinations_.begin());
        success_sent_flag_ = 0;
    }
    return 0;
}
int Migrate::MigrateIn(Population &population, EA_CPU *EA_CPU)
{
    int count = UpdatePopulation(population);
    UpdateDestination();
    UpdatePopulationBuffer(population, EA_CPU);
    return count;
}
int Migrate::MigrateOut()
{
    CheckAndRecvEmigrations();
    SendEmigrations();

    return 0;
}

int Migrate::DeserialMsgToIndividual(vector<Individual> &individual, double *msg, int length)
{
    int count = 0;

    for (int i = 0; i < length; i++)
    {
        Individual local_individual;
        for(int j = 0; j < problem_info_.dim; j++)
        {
            local_individual.elements.push_back(msg[count]);
            count++;
        }
        local_individual.fitness_value = msg[count];
        count++;
        individual.push_back(local_individual);
    }
    return 0;
}


int Migrate::SerialIndividualToMsg(double *msg, vector<Individual> &individual)
{
    int count = 0;
    for (int i = 0; i < individual.size(); i++)
    {
        for (int j = 0; j < problem_info_.dim; j++)
        {
            msg[count] = individual[i].elements[j];
            count++;
        }
        msg[count] = individual[i].fitness_value;
        count++;
    }
    return 0;
}

int Migrate::Finish()
{
    MPI_Status mpi_status;
    int tag = 10 * problem_info_.run_ID;

    while(success_sent_flag_ == 0)
    {
        MPI_Test(&mpi_request_, &success_sent_flag_, &mpi_status);
        CheckAndRecvEmigrations();
    }
    int flag_finish = 1;
    for(int i = 0; i < node_info_.node_num; i++)
        if(i != node_info_.node_ID)
            MPI_Send(&flag_finish, 1, MPI_INT, i, tag + FLAG_FINISH, MPI_COMM_WORLD);
        
    int sum_flag_finish = 1;
    while(sum_flag_finish != node_info_.node_num)
    {
        int flag = 0;

        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &mpi_status);
        if(flag == 1)
        {
            if(mpi_status.MPI_TAG == tag + EMIGRATIONS_ISLAND)
            {
                int count = 0;
                MPI_Get_count(&mpi_status, MPI_DOUBLE, &count);

                int message_length = count;
                real * msg_recv = new real[message_length];
                MPI_Recv(msg_recv, message_length, MPI_DOUBLE, mpi_status.MPI_SOURCE, mpi_status.MPI_TAG, MPI_COMM_WORLD, &mpi_status);

                delete [] msg_recv;
            }
            else if(mpi_status.MPI_TAG == tag + FLAG_FINISH)
            {
                int msg_recv = 0;
                MPI_Recv(&msg_recv, 1, MPI_INT, mpi_status.MPI_SOURCE, mpi_status.MPI_TAG, MPI_COMM_WORLD, &mpi_status);
                if(msg_recv == 1)
                    sum_flag_finish++;
            }

        }
    }
    return 0;
}