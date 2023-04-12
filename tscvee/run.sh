#!/bin/sh
rm -rf ./run.log
./make.sh
cd bin
./tsc-vee > ../run.log 2>&1