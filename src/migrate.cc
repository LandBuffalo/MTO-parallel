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
    
    for(int i = 0; i < problem_info.task_num; i++)
    {
        Population tmp_population;
        recv_buffer_.push_back(tmp_population);
    }
    
    success_sent_flag_ = 1;
    return 0;
}

int Migrate::Unitilize()
{
    delete []send_msg_to_other_EA_;
    delete buffer_manage_;
    recv_buffer_.clear();
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
        int source_task_ID = node_info_.task_IDs[mpi_status.MPI_SOURCE];
        recv_buffer_[source_task_ID] = emigration_import;
        delete [] msg_recv;
    }

    return 0;
}

int Migrate::UpdateBuffer(EA_CPU *EA_CPU)
{
    int extra_FEs = 0;
    Population imported_to_buffer_individuals;

    for(int i = 0; i < recv_buffer_.size(); i++)
    {
        for(size_t j = 0; j < recv_buffer_[i].size(); j++)
        {
            int task_ID = node_info_.task_IDs[node_info_.node_ID];
            
            recv_buffer_[i][j].fitness_value = EA_CPU->EvaluateFitness(recv_buffer_[i][j], task_ID);
            imported_to_buffer_individuals.push_back(recv_buffer_[i][j]);
            extra_FEs++;
        }
        recv_buffer_[i].clear();

    }
    buffer_manage_->UpdateBuffer(imported_to_buffer_individuals);
    return extra_FEs;
}

int Migrate::SendEmigrations(Population &population)
{
    MPI_Status mpi_status;

    if(success_sent_flag_ == 0)
    {
        MPI_Test(&mpi_request_, &success_sent_flag_, &mpi_status);
    }
    if (success_sent_flag_ == 1 && destinations_.size() > 0)
    {
        Population emigration_export;
#ifdef NO_INTERACTION        
        if (node_info_.task_IDs[destinations_[0]] != node_info_.task_IDs[node_info_.node_ID])
            return 0;
#endif
        vector<real> tmp_fitness_values;

        for(int i = 0; i < island_info_.island_size; i++)
            tmp_fitness_values.push_back(population[i].fitness_value);

        for(int i = 0; i < island_info_.migration_size; i++)
        {
            real best_fitness_value = tmp_fitness_values[0];
            int best_ID = 0;   
            for(int j = 1; j < island_info_.island_size; j++)
            {
                if (tmp_fitness_values[j] < best_fitness_value)
                {
                    best_fitness_value = tmp_fitness_values[j];
                    best_ID = j;
                }
            }
            tmp_fitness_values[best_ID] = 1e20;
            emigration_export.push_back(population[best_ID]);
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
    int extra_FEs = UpdateBuffer(EA_CPU);

    UpdatePopulation(population);
    UpdateDestination();
    return extra_FEs;
}
int Migrate::MigrateOut(Population &population)
{
    CheckAndRecvEmigrations();
    SendEmigrations(population);

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