# CMakeLists.txt cmake configuration for g2log test
# g2log is a KjellKod Logger
# 2011 @author Kjell Hedström, hedstrom@kjellkod.cc 
# ==================================================================
# 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own
#    risk and comes  with no warranties. 
# 
# This code is yours to share, use and modify with no strings attached
#   and no restrictions or obligations.
# ===================================================================

# Below are details for compiling on Windows and Linux
# by default only an example g2log binary is created
# the performance and unit tests creation can be enabled by switching their
# OPTIONs from OFF to ON --- See below at around line 110

# WINDOWS == README: Example how to setup environment + running an example
# 1. please use the "Visual Studio Command Prompt 2010)"
# 2. from the g2log folder
#    mkdir build
#    cd build;
# 3. cmake -G "Visual Studio 10" ..
# 4. msbuild g2log_by_kjellkod.sln
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
set(DIR_UNIT_TEST ${g2log_by_kjellkod_SOURCE_DIR}/test_unit)
set(DIR_EXAMPLE ${g2log_by_kjellkod_SOURCE_DIR}/test_example)
set(DIR_PERFORMANCE ${g2log_by_kjellkod_SOURCE_DIR}/test_performance)

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

   # -rdynamic is needed for correct stack dumps with demangling
   set(CMAKE_CXX_FLAGS "-Wall -rdynamic -Wunused -std=c++0x ${CMAKE_CXX_FLAGS_DEBUG} -pthread -I/usr/include/justthread")
   set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_unix.cpp)
   include_directories("/usr/include/justthread")
   ENDIF(UNIX)


#Visual Studio 2010
IF(WIN32)   	
   MESSAGE("")
   MESSAGE("Windows: Please run the command [cmake -G \"Visual Studio 10\" ..]")
   MESSAGE("if cmake finishes OK, do 'msbuild g2log_by_kjellkod.sln'")
   MESSAGE("then run 'Debug\\g2log-example.exe'  or whatever performance test you feel like trying")
   MESSAGE("")
   set(PLATFORM_LINK_LIBRIES $ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/lib/justthread_vc10_mdd.lib)
   set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_win.cpp)
   include_directories("$ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/include")
ENDIF(WIN32)



   # GENERIC STEPS
   # CODE SOURCES these + 
   set(SRC_CPP ${LOG_SRC}/g2logworker.cpp ${LOG_SRC}/g2log.cpp)
   set(SRC_H ${LOG_SRC}/g2logworker.h ${LOG_SRC}/g2log.h ${LOG_SRC}/crashhandler.h)
   set(SRC_FILES ${SRC_CPP} ${SRC_H} ${SRC_PLATFORM_SPECIFIC})

	

     # add a ActiveObject library
   add_library(lib_activeobject ${LOG_SRC}/active.cpp ${LOG_SRC}/active.h ${LOG_SRC}/shared_queue.h)
   set_target_properties(lib_activeobject PROPERTIES LINKER_LANGUAGE CXX)
	 
   # add a g2log library
   include_directories(src)
   include_directories(${LOG_SRC})
   MESSAGE("  LOG_SRC = : ${LOG_SRC}")
   MESSAGE("  g2logger files: [${SRC_FILES}]")
   add_library(lib_g2logger  ${SRC_FILES})
   set_target_properties(lib_g2logger PROPERTIES LINKER_LANGUAGE CXX)
   target_link_libraries(lib_g2logger lib_activeobject)



   # ============================================================================
   # OPTIONS: Turn OFF the ones that is of no interest to you
   # ---- by default all is OFF: except 'g2log-example -----
   # ---- the reason for this is that 
   # ----- 1) the performance tests were only thoroughly tested on Ubuntu, not windows-
   #           (g2log windows/linux, but Google's glog only on linux)
   #
   #       2) The unit test were tested windows/linux,. but must be unzipped
   #          before it can be "cmake'd" and compiled --- leaving it as OFF for now
   # ============================================================================
   # 1. a simple test example 'g2log-example'
   option (USE_SIMPLE_EXAMPLE 
          "Create a simple binary that runs a few LOG calls" ON)

   # 2. performance test (average + worst case) for KjellKod's g2log
   option (USE_G2LOG_PERFORMANCE
         "Create performance tests for g2log" OFF)

   # 3. performance test for Google's glog
   option (USE_GOOGLE_GLOG_PERFORMANCE
         "Create performance tests for Google's glog (remember to install glog, snapshot available at g2log/3rdParty/glog)" OFF)

   # 4. unit test for g2log
   option (USE_G2LOG_UNIT_TEST
          "Create unit test for g2log (remember to unzip Google's gtest framework, available at g2log/3rdParty/gtest)" OFF)




   # ========================================================
   #
   # BELOW : Creating executables depending on OPTIONS above
   #
   #  =======================================================
   

   # 1. create the the example EXECUTABLE - hook in the test_example's CMakeLists.txt file
   if (USE_SIMPLE_EXAMPLE)
      MESSAGE(" g2log-example option ON")
      include_directories (${DIR_EXAMPLE})
      add_executable(g2log-example ${DIR_EXAMPLE}/main.cpp)
      target_link_libraries(g2log-example lib_activeobject lib_g2logger ${PLATFORM_LINK_LIBRIES})
   endif (USE_SIMPLE_EXAMPLE) 
   # =========================


   # 2. create the g2log's performance tests
   if (USE_G2LOG_PERFORMANCE)
      MESSAGE(" g2log performance tests option ON")
      include_directories (${DIR_PERFORMANCE}) 
      # MEAN PERFORMANCE TEST
      add_executable(g2log-performance-threaded_mean 
                    ${DIR_PERFORMANCE}/main_threaded_mean.cpp ${DIR_PERFORMANCE}/performance.h)
      # Turn on G2LOG performance flag
      set_target_properties(g2log-performance-threaded_mean PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
      target_link_libraries(g2log-performance-threaded_mean lib_activeobject lib_g2logger ${PLATFORM_LINK_LIBRIES})

     # WORST CASE PERFORMANCE TEST
     add_executable(g2log-performance-threaded_worst ${DIR_PERFORMANCE}//main_threaded_worst.cpp ${DIR_PERFORMANCE}/performance.h)
     # Turn on G2LOG performance flag
     set_target_properties(g2log-performance-threaded_worst  PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
     target_link_libraries(g2log-performance-threaded_worst lib_activeobject lib_g2logger ${PLATFORM_LINK_LIBRIES})
   endif (USE_G2LOG_PERFORMANCE)
     # =========================



   # 3. create the Google glog's performance test
   if (USE_GOOGLE_GLOG_PERFORMANCE) 
      MESSAGE(" Google's glog performance tests option ON")
      include_directories (${DIR_PERFORMANCE}) 
	#Linux is easy!
    if(UNIX)
       set(GLOG_LIB glog)
	   # create the the GOOGLE MEAN_PERFORMANCE executable
       add_executable(google_glog-performance-threaded_mean ${DIR_PERFORMANCE}/main_threaded_mean.cpp ${DIR_PERFORMANCE}/performance.h)
       set_target_properties(google_glog-performance-threaded_mean  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
       target_link_libraries(google_glog-performance-threaded_mean lib_activeobject ${GLOG_LIB} ${PLATFORM_LINK_LIBRIES})
	 
       # create the the GOOGLE MEAN_PERFORMANCE executable
       add_executable(google_glog-performance-threaded_worst ${DIR_PERFORMANCE}/main_threaded_worst.cpp ${DIR_PERFORMANCE}/performance.h)
       set_target_properties(google_glog-performance-threaded_worst  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
  	   target_link_libraries(google_glog-performance-threaded_worst lib_activeobject  ${GLOG_LIB} ${PLATFORM_LINK_LIBRIES})
	 endif(UNIX)
	
	 # GLOG on Linux is easy - but for Windows trickier,. and it doesn't work (as of yet)
     if(WIN32)
       MESSAGE("******************************************************")
	   MESSAGE("*** SORRY- Google glog on windows is not preconfigured")
	   MESSAGE("*** You have to do this yourself: ref CMakeLists.txt")
       MESSAGE("******************************************************")
	   MESSAGE("")
	    #set(GLOG_DIR ../3rdParty/glog/glog-0.3.1)   
        #include_directories(${GLOG_DIR}/src/windows)
     endif(WIN32)	  


   endif (USE_GOOGLE_GLOG_PERFORMANCE)
   # ===========================


   # 4. create the unit tests for g2log --- ONLY TESTED THE UNIT TEST ON LINUX
   if (USE_G2LOG_UNIT_TEST)  
     MESSAGE(" g2log unit testing option ON")
    # SETUP for GTEST
      set(GTEST_DIR ../3rdParty/gtest/gtest-1.6.0__stripped)
      set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR} ${GTEST_DIR}/src)
      include_directories(${GTEST_INCLUDE_DIRECTORIES})
      add_library(gtest_160_lib ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc)
      enable_testing(true)

      add_executable(g2log-unit_test ../test_main/test_main.cpp ${DIR_UNIT_TEST}/test_io.cpp)
      target_link_libraries(g2log-unit_test lib_activeobject lib_g2logger gtest_160_lib ${PLATFORM_LINK_LIBRIES})
   endif (USE_G2LOG_UNIT_TEST)



	


