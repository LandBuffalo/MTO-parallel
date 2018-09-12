#!/bin/bash 
cd ./EA_library/
bash make_EA.sh
cd ..
mv libEA_CPU.a ./src
mv libCEC2014.a ./src
mv libDE_CPU.a ./src
make clean
make

