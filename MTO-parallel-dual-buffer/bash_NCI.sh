#!/bin/bash 
export LC_ALL=en_US
cd ./EA_library/
bash make_EA.sh
cd ..
mv liball_in_one_CPU.a ./src
make clean
make

