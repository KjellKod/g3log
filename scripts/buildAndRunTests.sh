#!/bin/bash

set -ev

unzip -o 3rdParty/gtest/gtest-1.7.0.zip -d 3rdParty/gtest
mkdir  build_travis
cd build_travis
cmake -DCMAKE_CXX_COMPILER=g++ -DUSE_G3LOG_UNIT_TEST=ON ..
make -j
echo "Testing with g++"
./test_concept_sink
./test_configuration
./test_dynamic_loaded_shared_lib
./test_filechange
./test_io
./test_sink


echo "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
echo "Testing with Clang++"
make clean
rm -rf *Make* *make* *.so  *3log*
cmake -DCMAKE_CXX_COMPILER=clang++ -DUSE_G3LOG_UNIT_TEST=ON ..
make -j
./test_concept_sink
./test_configuration
#./test_dynamic_loaded_shared_lib
./test_filechange
./test_io
./test_sink
