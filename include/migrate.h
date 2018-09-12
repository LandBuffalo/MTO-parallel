#ifndef __MIGRATE_HH__
#define __MIGRATE_HH__
#include <mpi.h>
#include "random.h"
#include "config.h"
#include "buffer_manage.h"
#include "EA_CPU.h"
class Migrate
{
private:
    Random                  random_;
    IslandInfo              island_info_;
    ProblemInfo             problem_info_;
    NodeInfo                node_info_;
    
    MPI_Request             mpi_request_;
    int                     success_sent_flag_;

    int                     total_insert_;
    int                     total_success_insert_;

    real *                  send_msg_to_other_EA_;
    BufferManage *          buffer_manage_;
    vector<int>             destinations_;
    vector< vector<real> >  v_original_fitness_values_;
    Population              population_buffer_;
    int                     RegroupIslands(Population &population);
    vector<int>             FindBestIndividualInIsland(Population &population);
    int                     SerialIndividualToMsg(real *msg_of_node_EA, vector<Individual> &individual);
    int                     DeserialMsgToIndividual(vector<Individual> &individual, real *msg_of_node_EA, int length);
    int                     UpdatePopulationBuffer(Population &population, EA_CPU *EA_CPU);
    int                     UpdatePopulation( Population & population);
    int                     CheckAndRecvEmigrations();
    int                     UpdateDestination();
    int                     SendEmigrations();
public:
                            Migrate();
                            ~Migrate();
    int                     Initilize(IslandInfo island_info, ProblemInfo problem_info, NodeInfo node_info);
    int                     Unitilize();
    int                     Finish();
    int                     TotalInsert();
    int                     TotalSuccessInsert();
    double                  SuccessRate();
    int                     MigrateIn(Population &population, EA_CPU *EA_CPU);
    int                     MigrateOut();
};
#endif
