# CMakeLists.txt cmake configuration for g2log test
# g2log is a KjellKod Logger
# 2011 @author Kjell Hedström, hedstrom@kjellkod.cc */
#


# WINDOWS == README: Example how to setup environment + running an example
# 1. please use the "Visual Studio Command Prompt 2010)"
# 2. from the g2log folder
#    mkdir build
#    cd build;
# 3. cmake -G "Visual Studio 10" ..
# 4. msbuild g2log-example.sln
# 5. Debug\g2log-example.exe
#

# LINUX == README: Example how to setup environment + running tests for ===============
# 1. Install gtest
#    cmake
#    make
#    make install (possibly as root)
#
# 2. update path to libraries
#    sudo /sbin/ldconfig -v   | grep gtest
#
#    the grep is only to verify that it works. It should give something like
#   ... other stuff ...
#      	libgtest.so.0 -> libgtest.so.0.0.0
#	libgtest_main.so.0 -> libgtest_main.so.0.0.0
#
# 3. LINUX:To try this out from folder g2log:
#    mkdir build
#    cd build
#    cmake ..    # create makefiles in g2log/build directory
#    make    # link active_object, g2log and example code to get an "example" executable
#     ./g2log-example
#   
# ============================================================================

cmake_minimum_required (VERSION 2.6)
project (g2log_by_kjellkod)
set(LOG_SRC ${g2log_by_kjellkod_SOURCE_DIR}/src)
MESSAGE("  LOG_SRC = : ${LOG_SRC}")
include_directories(${LOG_SRC})
SET(ACTIVE_CPP0xx_DIR "Release")



IF(UNIX)
   MESSAGE("")
   MESSAGE("cmake for *NIX ")
   MESSAGE("if cmake finishes OK, do make")
   MESSAGE("then run './g2log-example' or whatever performance test you feel like trying")
   MESSAGE("")
   set(PLATFORM_LINK_LIBRIES justthread rt)
   set(CMAKE_CXX_FLAGS "-Wall -Wunused -std=c++0x ${CMAKE_CXX_FLAGS_DEBUG} -pthread -I/usr/include/justthread")
   set(G2_LOG_FILES ${LOG_SRC}/logworker.h ${LOG_SRC}/logworker.cpp ${LOG_SRC}/g2log.h ${LOG_SRC}/g2log.cpp ${LOG_SRC}/crashhandler.h ${LOG_SRC}/crashhandler_unix.cpp)
   include_directories("/usr/include/justthread")

    # SETUP for GTEST
   set(GTEST_DIR ../3rdParty/gtest/gtest-1.6.0__stripped)
   set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR} ${GTEST_DIR}/src)
   include_directories(${GTEST_INCLUDE_DIRECTORIES})
   add_library(gtest_160_lib ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc)
   enable_testing(true)
ENDIF(UNIX)

#Visual Studio 2010
IF(WIN32)   	
   MESSAGE("")
   MESSAGE("cmake for Visual Studio 2010")
   MESSAGE("if cmake finishes OK, do 'msbuild g2log_by_kjellkod.sln'")
   MESSAGE("then run 'Debug\\g2log-example.exe'  or whatever performance test you feel like trying")
   MESSAGE("")
   set(PLATFORM_LINK_LIBRIES $ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/lib/justthread_vc10_mdd.lib)
   set(G2_LOG_FILES ${LOG_SRC}/logworker.h ${LOG_SRC}/logworker.cpp ${LOG_SRC}/g2log.h ${LOG_SRC}/g2log.cpp ${LOG_SRC}/crashhandler.h ${LOG_SRC}/crashhandler_win.cpp)
   include_directories("$ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/include")
ENDIF(WIN32)



# GENERIC STEPS
     # add a ActiveObject library
   add_library(lib_activeobject ${LOG_SRC}/active.cpp ${LOG_SRC}/active.h ${LOG_SRC}/shared_queue.h)
   set_target_properties(lib_activeobject PROPERTIES LINKER_LANGUAGE CXX)
	 
   # add a g2log library
   include_directories(src)
   include_directories(${LOG_SRC})
   MESSAGE("  LOG_SRC = : ${LOG_SRC}")
   MESSAGE("  g2logger files: [${G2_LOG_FILES}]")
   add_library(lib_g2logger  ${G2_LOG_FILES})
   set_target_properties(lib_g2logger PROPERTIES LINKER_LANGUAGE CXX)
   target_link_libraries(lib_g2logger lib_activeobject)


   # create the the example EXECUTABLE
   add_executable(g2log-example src/main.cpp)
   target_link_libraries(g2log-example lib_activeobject lib_g2logger ${PLATFORM_LINK_LIBRIES})




	
   #  Below are g2log unit testTEST
   #    and PERFORMANCE comparisons between g2log and google's glog
   #
   #
   # ---- Below g2log Performance -----
   # create the the g2log TWO_THREADS_MEAN_PERFORMANCE executable
   add_executable(g2log-performance-threaded_mean test/main_threaded_mean.cpp test/performance.h)
   set_target_properties(g2log-performance-threaded_mean PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
   target_link_libraries(g2log-performance-threaded_mean lib_activeobject lib_g2logger ${PLATFORM_LINK_LIBRIES})

   # create the the g2log TWO_THREADS_WORST_CASE_PERFORMANCE executable
   add_executable(g2log-performance-threaded_worst test/main_threaded_worst.cpp test/performance.h)
   set_target_properties(g2log-performance-threaded_worst  PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
   target_link_libraries(g2log-performance-threaded_worst lib_activeobject lib_g2logger ${PLATFORM_LINK_LIBRIES})




IF(UNIX)
   #
   # ---- Below GOOGLE glog Performance -----
   # create the the GOOGLE MEAN_PERFORMANCE executable
   add_executable(google_glog-performance-threaded_mean test/main_threaded_mean.cpp test/performance.h)
   set_target_properties(google_glog-performance-threaded_mean  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
   target_link_libraries(google_glog-performance-threaded_mean lib_activeobject glog ${PLATFORM_LINK_LIBRIES})

   # create the the GOOGLE MEAN_PERFORMANCE executable
   add_executable(google_glog-performance-threaded_worst test/main_threaded_worst.cpp test/performance.h)
   set_target_properties(google_glog-performance-threaded_worst  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
   target_link_libraries(google_glog-performance-threaded_worst lib_activeobject glog ${PLATFORM_LINK_LIBRIES})


   # create the the TEST executable
   add_executable(g2log-unit_test ../test_main/test_main.cpp test/test_io.cpp)
   add_library(lib_test_logger  ${LOG_SRC}/logworker.h ${LOG_SRC}/logworker.cpp ${LOG_SRC}/g2log.h ${LOG_SRC}/g2log.cpp ${LOG_SRC}/crashhandler.h ${LOG_SRC}/crashhandler_unix.cpp)
   set_target_properties(lib_test_logger PROPERTIES LINKER_LANGUAGE CXX)
   set_target_properties(lib_test_logger PROPERTIES COMPILE_DEFINITIONS "YALLA=1")
   target_link_libraries(lib_test_logger lib_activeobject)
   # For unit-test only, dont 'crash' by signals, instead just throw a std::runtime_error
   set_target_properties(g2log-unit_test PROPERTIES COMPILE_DEFINITIONS "YALLA=1")
   target_link_libraries(g2log-unit_test lib_activeobject lib_test_logger gtest_160_lib justthread rt)
ENDIF(UNIX)

