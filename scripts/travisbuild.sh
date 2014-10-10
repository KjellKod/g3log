#!/bin/bash

set -ev

unzip -fo 3rdParty/gtest/gtest-1.7.0.zip -d 3rdParty/gtest
mkdir build_travis
cd build_travis
cmake -DUSE_G3LOG_UNIT_TEST=ON ..
make -j
./test_concept_sink

