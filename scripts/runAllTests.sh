#!/bin/bash

set -v
#set -x


# test_execs=`find ./test_* -perm /u=x` 
test_execs=`find ./test_* -type f -perm +ugo+x -print`

echo "Tests to run: $test_execs"

while read -r testsuite; do
    ./"$testsuite"
    if [ "$?" -ne 0 ]; then
       echo "Aborting. \"$testsuite\" had failing test(s)"
       exit 1
    fi
done <<< "$test_execs"


#option 2
#find ./test_* -perm /u=x -exec '{}' ';'

# option 3
#./test_concept_sink && \
#./test_cpp_future_concepts && \
#./test_dynamic_loaded_shared_lib | true && \
#./test_filechange && \
#./test_io && \
#./test_sink && \
#./test_message
