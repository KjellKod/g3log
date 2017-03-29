#!/bin/bash

set -ev
set -x

./test_concept_sink && \
./test_cpp_future_concepts && \
./test_dynamic_loaded_shared_lib | true && \
./test_filechange && \
./test_io && \
./test_sink && \
./test_message
