
g++ -c  EA_CPU.cc 
g++ -c  DE_CPU.cc 
g++ -c  CEC2014.cc 

ar rvs  libDE_CPU.a DE_CPU.o
ar rvs  libEA_CPU.a EA_CPU.o
ar rvs  libCEC2014.a CEC2014.o

mv -f libDE_CPU.a ../
mv -f libEA_CPU.a ../
mv -f libCEC2014.a ../

rm -f *.o