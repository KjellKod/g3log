#!/bin/bash

set -ev

unzip -o 3rdParty/gtest/gtest-1.7.0.zip -d 3rdParty/gtest


if [ "$CXX" = "g++" ]; then export CXX=g++-4.8; fi
if [ "$CXX" = "clang++" ]; then export CXX=clang++-3.4; fi
echo $TRAVIS_OS_NAME
echo $CXX


mkdir -p  build_travis
cd build_travis

if [[ $CXX == *"g++"* ]]
then
   echo "Testing with g++"
   cmake -DUSE_DYNAMIC_LOGGING_LEVELS=ON -DADD_G3LOG_UNIT_TEST=ON ..
    make -j
   ./test_concept_sink
   ./test_configuration
   ./test_dynamic_loaded_shared_lib
   ./test_filechange
   ./test_io
   ./test_sink
fi

if [ "$CXX" = "clang++-3.4" ]
then
    echo "Testing with Clang++"
    cmake -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS=-std=gnu++11 -DUSE_G3LOG_UNIT_TEST=ON ..
    make -j
    ./test_concept_sink
    ./test_configuration
    #./test_dynamic_loaded_shared_lib
    ./test_filechange
    ./test_io
    ./test_sink
 fi
