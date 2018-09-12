qsub -lncpus=32 -v ISLAND_SIZE=32,PROBLEM_ID=1 Scalable_EA.pbs
qsub -lncpus=64 -v ISLAND_SIZE=32,PROBLEM_ID=2 Scalable_EA.pbs
qsub -lncpus=64 -v ISLAND_SIZE=32,PROBLEM_ID=3 Scalable_EA.pbs
qsub -lncpus=64 -v ISLAND_SIZE=32,PROBLEM_ID=4 Scalable_EA.pbs
qsub -lncpus=64 -v ISLAND_SIZE=32,PROBLEM_ID=5 Scalable_EA.pbs