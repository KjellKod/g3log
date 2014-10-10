#!/bin/bash

set -ev
cd ..
unzip 3rdParty/gtest/gtest-1.7.0.zip -d 3rdParty/gtest
mkdir travisbuild
cd travisbuild
cmake -DUSE_G3LOG_UNIT_TEST=ON ..
make -j
./test_concept_sink

