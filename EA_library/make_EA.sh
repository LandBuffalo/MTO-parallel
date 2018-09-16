
g++ -c  -I../include EA_CPU.cc 
g++ -c  -I../include DE_CPU.cc 
g++ -c  -I../include CEC2014.cc 

ar rvs  libDE_CPU.a DE_CPU.o
ar rvs  libEA_CPU.a EA_CPU.o
ar rvs  libCEC2014.a CEC2014.o
ar cru  liball_in_one_CPU.a *.o

mv -f libDE_CPU.a ../
mv -f libEA_CPU.a ../
mv -f libCEC2014.a ../
mv -f liball_in_one_CPU.a ../

rm -f *.o