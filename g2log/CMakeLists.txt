# CMakeLists.txt cmake configuration for g2log test
# 2011 @author Kjell Hedström, hedstrom@kjellkod.cc */
#
# g2log is a KjellKod Logger
#
# == README: Example how to setup environment + running tests ===============
# 1. Install gtest
#    cmake
#    make
#    make install (possibly as root)
#
#
#
# 2. update path to libraries
#    sudo /sbin/ldconfig -v   | grep gtest
#
#    the grep is only to verify that it works. It should give something like
#       ... other stuff ...
#      	libgtest.so.0 -> libgtest.so.0.0.0
#	libgtest_main.so.0 -> libgtest_main.so.0.0.0
#
#
#
# 3. To try this out from folder g2log:
#    mkdir build
#    cd build
#    cmake ..    # create makefiles in g2log/build directory
#    make        # link active_object, g2log and example code to get an "example" executable
#                  ./g2log-example
#   
# ============================================================================

cmake_minimum_required (VERSION 2.6)
project (kjellkod_logger)
set(LOG_SRC ${kjellkod_logger_SOURCE_DIR}/src)
MESSAGE("  LOG_SRC = : ${LOG_SRC}")

IF(UNIX)
    set(CMAKE_CXX_FLAGS "-Wall -Wunused -std=c++0x ${CMAKE_CXX_FLAGS_DEBUG} -pthread -I/usr/include/justthread")

    # SETUP for GTEST
    set(GTEST_DIR ../3rdParty/gtest/gtest-1.6.0__stripped)
    set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR} ${GTEST_DIR}/src)
    include_directories(${GTEST_INCLUDE_DIRECTORIES})
    add_library(gtest_160_lib ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc)
    enable_testing(true) 


     # make the src directory available
     include_directories(/usr/include/justthread)   #not necessarily needed if it's in the path

     # add a ActiveObject library
     set(ACTIVE_DIR ${LOG_SRC})
     include_directories(${ACTIVE_DIR})
     MESSAGE("  ACTIVE_DIR = : ${ACTIVE_DIR}")
     SET(ACTIVE_CPP0xx_DIR "Release")
     add_library(lib_activeobject ${ACTIVE_DIR}/active.cpp ${ACTIVE_DIR}/active.h ${ACTIVE_DIR}/shared_queue.h)
     set_target_properties(lib_activeobject PROPERTIES LINKER_LANGUAGE CXX)


     include_directories(src)
     include_directories(${LOG_SRC})
     #MESSAGE("  LOG_SRC = : ${LOG_SRC}")
     add_library(lib_logger  ${LOG_SRC}/logworker.h ${LOG_SRC}/logworker.cpp ${LOG_SRC}/g2log.h ${LOG_SRC}/g2log.cpp )
     set_target_properties(lib_logger PROPERTIES LINKER_LANGUAGE CXX)
     target_link_libraries(lib_logger lib_activeobject)

     # create the the example EXECUTABLE
     add_executable(g2log-example src/main.cpp)
     # link executable with the src library
    target_link_libraries(g2log-example lib_activeobject lib_logger justthread rt)


   #  Below are g2log unit testTEST
   #        and PERFORMANCE comparisons between g2log and google's glog
   #
   #
   include_directories(build)

     # create the the TEST executable
    add_executable(g2log-unit_test ../test_main/test_main.cpp test/test_io.cpp)
    target_link_libraries(g2log-unit_test lib_activeobject lib_logger gtest_160_lib justthread rt)

    # ---- Below g2log Performance -----
     # create the the g2log MEAN_PERFORMANCE executable
    add_executable(g2log-performance-mean test/main_mean.cpp test/performance.h)
    set_target_properties(g2log-performance-mean PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
    target_link_libraries(g2log-performance-mean lib_activeobject lib_logger justthread rt)

     # create the the g2log TWO_THREADS_MEAN_PERFORMANCE executable
    add_executable(g2log-performance-2threads_mean test/main_2threads_mean.cpp test/performance.h)
    set_target_properties(g2log-performance-2threads_mean PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
    target_link_libraries(g2log-performance-2threads_mean lib_activeobject lib_logger justthread rt)

    # create the the g2log TWO_THREADS_WORST_CASE_PERFORMANCE executable
    add_executable(g2log-performance-2threads_worst test/main_2threads_worst.cpp test/performance.h)
    set_target_properties(g2log-performance-2threads_worst  PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
    target_link_libraries(g2log-performance-2threads_worst lib_activeobject lib_logger justthread rt)

    #
    # ---- Below GOOGLE glog Performance -----
    # create the the GOOGLE MEAN_PERFORMANCE executable
    # Generate the DEFINE (for glog) needed to differentiate between the glog and the g2log test
    add_executable(google_glog-performance-mean test/main_mean.cpp test/performance.h)
    set_target_properties(google_glog-performance-mean  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
    target_link_libraries(google_glog-performance-mean lib_activeobject glog justthread rt)

    # create the the GOOGLE MEAN_PERFORMANCE executable
    add_executable(google_glog-performance-2threads_mean test/main_2threads_mean.cpp test/performance.h)
    set_target_properties(google_glog-performance-2threads_mean  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
    target_link_libraries(google_glog-performance-2threads_mean lib_activeobject glog justthread rt)

    # create the the GOOGLE MEAN_PERFORMANCE executable
    add_executable(google_glog-performance-2threads_worst test/main_2threads_worst.cpp test/performance.h)
    set_target_properties(google_glog-performance-2threads_worst  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
    target_link_libraries(google_glog-performance-2threads_worst lib_activeobject glog justthread rt)





ENDIF(UNIX)

