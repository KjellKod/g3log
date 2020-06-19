#!/bin/bash

set -ev
set -x
unzip -o 3rdParty/gtest/gtest-1.7.0.zip -d 3rdParty/gtest


mkdir -p  build_travis
cd build_travis
cmake  -DCMAKE_CXX_FLAGS=-std=c++11 -DADD_G3LOG_UNIT_TEST=ON ..
#        LINUX                            OR    OSX
makeArg=`grep -c ^processor /proc/cpuinfo || sysctl -n hw.ncpu`
make -j$makeArg
ctest -V 

