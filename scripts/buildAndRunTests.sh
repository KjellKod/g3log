#!/bin/bash

set -ev
set -x
unzip -o 3rdParty/gtest/gtest-1.7.0.zip -d 3rdParty/gtest


mkdir -p  build_travis
cd build_travis
cmake -DCMAKE_CXX_FLAGS=-std=c++14 -DADD_G3LOG_BENCH_PERFORMANCE=ON -DADD_G3LOG_UNIT_TEST=ON -DCMAKE_INSTALL_PREFIX=./install -DCPACK_PACKAGING_INSTALL_PREFIX=/opt/g3log ..
cmake --build . --target install

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
    cpack -G "ZIP"
    unzip g3log-*-Darwin.zip
fi

if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then
    cpack -G "DEB;TGZ"
    tar zxvf g3log-*-Linux.tar.gz
fi

#makeArg=`grep -c ^processor /proc/cpuinfo`    

make -j
/bin/bash ../scripts/runAllTests.sh

