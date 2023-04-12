#!/bin/sh
echo "INFO: cleaning build file..."
rm -rf build
mkdir build
cd build
echo "INFO: generating makefile..."
cmake .. 
echo "INFO: compiling sourcecode..."
make > make.log 2>&1