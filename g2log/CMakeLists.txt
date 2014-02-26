#  CMakeLists.txt cmake configuration for g2log test
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

# === WINDOWS === 
# Example for: Visual Studio 2013 (earlier should work too)
# 1. please use the "Visual Studio Command Prompt 12 (2013)"
# 2. from the g2log folder
#    mkdir build
#    cd build;
# 3. cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 12" ..
# 4. msbuild g2log_by_kjellkod.sln /p:Configuration=Release
#
# Try to run an example, such as:
# 5. Release\g2log-FATAL-contract.exe
#
#

# ===   LINUX: === To try this out from folder g2log:
#    mkdir build
#    cd build
# >> create makefiles in g2log/build directory
#   cmake -DCMAKE_BUILD_TYPE=Release .. 
#    make -jN   (where N stands for number of cores you want to utilize)     
#
#
#
# ===  Clang on Linux ===
#   From g2log/g2log
#   mkdir build && cd build
#   cmake -DCMAKE_CXX_COMPILER=clang++ ..
#   if you want to double-check settings:   "VERBOSE=1 make" 
#   otherwise just run:   "make -j"                        
#                
# ============================================================================

cmake_minimum_required (VERSION 2.8)
ENABLE_LANGUAGE(CXX)
set(CMAKE_BUILD_TYPE Release)

project (g2log_by_kjellkod)
set(LOG_SRC ${g2log_by_kjellkod_SOURCE_DIR}/src)
set(DIR_UNIT_TEST ${g2log_by_kjellkod_SOURCE_DIR}/test_unit)
set(DIR_EXAMPLE ${g2log_by_kjellkod_SOURCE_DIR}/test_example)
set(DIR_PERFORMANCE ${g2log_by_kjellkod_SOURCE_DIR}/test_performance)

MESSAGE("  LOG_SRC = : ${LOG_SRC}")
include_directories(${LOG_SRC})
SET(ACTIVE_CPP0xx_DIR "Release")

# Detect 64 or 32 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
   # 64-bit project
   SET(64_BIT_OS TRUE)
     MESSAGE("A 64-bit OS detected")
 else()
   SET(64_BIT_OS FALSE)
   MESSAGE("A 32-bit OS detected")
endif()


#cmake -DCMAKE_CXX_COMPILER=clang++ ..
IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    MESSAGE("")
   MESSAGE("cmake for Clang ")
  # WARNING: If Clang for Linux does not work with full C++11 support it might be your
  # installation that is faulty. When I tested Clang on Ubuntu I followed the following
  # description 
  #  1) http://kjellkod.wordpress.com/2013/09/23/experimental-g2log-with-clang/
  #  2) https://github.com/maidsafe/MaidSafe/wiki/Hacking-with-Clang-llvm-abi-and-llvm-libc
  set(PLATFORM_LINK_LIBRIES rt  c++abi) 
  set(CMAKE_CXX_FLAGS  "-std=c++11 -pthread -stdlib=libc++ -D_GLIBCXX_USE_NANOSLEEP")
  set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_unix.cpp)
ELSEIF(UNIX)
   MESSAGE("")
   MESSAGE("cmake for *NIX ")
   MESSAGE("if cmake finishes OK, do make")
   MESSAGE("then run './g2log-FATAL-example' or whatever performance test you feel like trying")
   MESSAGE("")
   set(PLATFORM_LINK_LIBRIES rt)
   # -rdynamic is needed for correct stack dumps with demangling
   # -D_GLIBCXX_USE_NANOSLEEP is needed for this_thread sleep (unit testing)
   set(CMAKE_CXX_FLAGS "-Wall -rdynamic -Wunused -std=c++11 ${CMAKE_CXX_FLAGS_DEBUG} -pthread -D_GLIBCXX_USE_NANOSLEEP")
   set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_unix.cpp)
ELSEIF(MINGW)
   MESSAGE("")
   MESSAGE("cmake for MinGW ")
   MESSAGE("if cmake finishes OK, do make")
   MESSAGE("then run './g2log-FATAL-example' or whatever performance test you feel like trying")
   MESSAGE("")
   # -D_GLIBCXX_USE_NANOSLEEP is needed for this_thread sleep (unit testing)
   set(CMAKE_CXX_FLAGS "-Wall -Wunused -std=c++11 ${CMAKE_CXX_FLAGS_DEBUG} -pthread -D_GLIBCXX_USE_NANOSLEEP")
   set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_win.cpp)
ELSEIF(MSVC)
      # VC11 bug: http://code.google.com/p/googletest/issues/detail?id=408
      #          add_definition(-D_VARIADIC_MAX=10)
      # https://github.com/anhstudios/swganh/pull/186/files
      ADD_DEFINITIONS (/D_VARIADIC_MAX=10)
      MESSAGE(STATUS "- MSVC: Set variadic max to 10 for MSVC compatibility")
      # Remember to set set target properties if using GTEST similar to done below on target "unit_test"
      # "set_target_properties(unit_test  PROPERTIES COMPILE_DEFINITIONS "GTEST_USE_OWN_TR1_TUPLE=0")
   MESSAGE("")
   MESSAGE("Windows: Please run the command [cmake -DCMAKE_BUILD_TYPE=Release -G \"Visual Studio 11\" ..]")
   MESSAGE("if cmake finishes OK, do 'msbuild g2log_by_kjellkod.sln /p:Configuration=Release'")
   MESSAGE("then run 'Release\\g2log-FATAL-example.exe'  or whatever performance test you feel like trying")
   MESSAGE("")
   set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_win.cpp)
ENDIF()




   # GENERIC STEPS
   file(GLOB SRC_FILES ${LOG_SRC}/*.h ${LOG_SRC}/*.hpp ${LOG_SRC}/*.cpp ${LOG_SRC}/*.ipp)
   if(MSVC OR MINGW) 
         list(REMOVE_ITEM SRC_FILES  ${LOG_SRC}/crashhandler_unix.cpp)
   else()     
         list(REMOVE_ITEM SRC_FILES  ${LOG_SRC}/crashhandler_win.cpp)
   endif(MSVC OR MINGW)

   set(SRC_FILES ${SRC_FILES} ${SRC_PLATFORM_SPECIFIC})
 
   # Create the g2log library
   include_directories(${LOG_SRC})
   MESSAGE("  g3logger files: [${SRC_FILES}]")
   add_library(lib_g3logger  ${SRC_FILES})
   set_target_properties(lib_g3logger PROPERTIES LINKER_LANGUAGE CXX)



   # ============================================================================
   # G2LOG OPTIONS: Turn OFF the ones that is of no interest to you
   # ============================================================================
   option (USE_DYNAMIC_LOGGING_LEVELS
          "Turn ON/OFF log levels. An disabled level will not push logs of that level to the sink. By default dynamic logging is disabled" OFF)

   # Do 'cmake -DUSE_DYNAMIC_LOGGING_LEVELS=ON -DCMAKE_BUILD_TYPE=Release .. ' to enable this 
   #            (or change the default option above)
   if(USE_DYNAMIC_LOGGING_LEVELS)
     add_definitions(-DG2_DYNAMIC_LOGGING)
     MESSAGE ("\nUSE_DYNAMIC_LOGGING_LEVELS is turned on\n\tUse g2::setLogLevel(LEVEL boolean) to enable/disable logging on specified levels.\n\tAll levels are by default turn ON\n\n")
   endif(USE_DYNAMIC_LOGGING_LEVELS)


   # ============================================================================
   # TEST OPTIONS: Turn OFF the ones that is of no interest to you
   # ---- by default all is OFF: except 'g2log-FATAL-example -----
   # ---- the reason for this is that
   # ----- 1) the performance tests were only thoroughly tested on Ubuntu, not windows-
   #           (g2log windows/linux, but Google's glog only on linux)
   #
   #       2) The unit test were tested windows/linux,. but must be unzipped
   #          before it can be "cmake'd" and compiled --- leaving it as OFF for now
   # ============================================================================

   # 1. a simple test example 'g2log-FATAL-*'
   #    Do 'cmake -DUSE_SIMPLE_EXAMPLE=ON' to enable this 
   option (USE_SIMPLE_EXAMPLE  "Simple (fatal-crash/contract) examples " ON)

   # 2. performance test (average + worst case) for KjellKod's g2log
   #    Do 'cmake -DUSE_G2LOG_PERFORMANCE=ON' to enable this 
   option (USE_G2LOG_PERFORMANCE "g2log performance test" OFF)

   # 3. performance test for Google's glog
   #    remember to install glog, snapshot available at g2log/3rdParty/glog
   option (USE_GOOGLE_GLOG_PERFORMANCE  "Google's glog performance test" OFF) 
   

   # 4. unit test for g2log  (cmake -DUSE_G2LOG_UNIT_TEST=ON ..)
   #    remember to unzip gtest at g2log/3rdParty/gtest
   option (USE_G2LOG_UNIT_TEST "g2log unit tests" OFF)




   # ===============================================================================================
   #
   # BELOW : Creating executables depending on OPTIONS above
   #
   #  ==============================================================================================
   # 1. create the the example EXECUTABLE - hook in the test_example's CMakeLists.txt file
   # =========================
   if (USE_SIMPLE_EXAMPLE)
      MESSAGE(" g2log-FATAL-* examples option ON")
      include_directories (${DIR_EXAMPLE})
      add_executable(g2log-FATAL-contract ${DIR_EXAMPLE}/main_contract.cpp)
      add_executable(g2log-FATAL-sigsegv ${DIR_EXAMPLE}/main_sigsegv.cpp)
      target_link_libraries(g2log-FATAL-contract lib_g3logger ${PLATFORM_LINK_LIBRIES})
      target_link_libraries(g2log-FATAL-sigsegv lib_g3logger ${PLATFORM_LINK_LIBRIES})
   endif (USE_SIMPLE_EXAMPLE)



   # 2. create the g2log's performance tests
   # =========================
   if (USE_G2LOG_PERFORMANCE)
      MESSAGE(" g2log performance tests option ON")
      include_directories (${DIR_PERFORMANCE})
      # MEAN PERFORMANCE TEST
      add_executable(g2log-performance-threaded_mean
                    ${DIR_PERFORMANCE}/main_threaded_mean.cpp ${DIR_PERFORMANCE}/performance.h)
      # Turn on G2LOG performance flag
      set_target_properties(g2log-performance-threaded_mean PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
      target_link_libraries(g2log-performance-threaded_mean lib_g3logger ${PLATFORM_LINK_LIBRIES})

     # WORST CASE PERFORMANCE TEST
     add_executable(g2log-performance-threaded_worst ${DIR_PERFORMANCE}/main_threaded_worst.cpp ${DIR_PERFORMANCE}/performance.h)
     # Turn on G2LOG performance flag
     set_target_properties(g2log-performance-threaded_worst  PROPERTIES COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
     target_link_libraries(g2log-performance-threaded_worst  lib_g3logger ${PLATFORM_LINK_LIBRIES})
   endif (USE_G2LOG_PERFORMANCE)



   # 3. create the Google glog's performance test
   # =========================
   if (USE_GOOGLE_GLOG_PERFORMANCE)
      MESSAGE(" Google's glog performance tests option ON")
      include_directories (${DIR_PERFORMANCE})
        #Linux is easy!
    if(UNIX)
       set(GLOG_LIB glog)
       # create the the GOOGLE MEAN_PERFORMANCE executable
       add_executable(google_glog-performance-threaded_mean ${DIR_PERFORMANCE}/main_threaded_mean.cpp ${DIR_PERFORMANCE}/performance.h)
       set_target_properties(google_glog-performance-threaded_mean  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
       target_link_libraries(google_glog-performance-threaded_mean ${GLOG_LIB} ${PLATFORM_LINK_LIBRIES})

       # create the the GOOGLE MEAN_PERFORMANCE executable
       add_executable(google_glog-performance-threaded_worst ${DIR_PERFORMANCE}/main_threaded_worst.cpp ${DIR_PERFORMANCE}/performance.h)
       set_target_properties(google_glog-performance-threaded_worst  PROPERTIES COMPILE_DEFINITIONS "GOOGLE_GLOG_PERFORMANCE=1")
       target_link_libraries(google_glog-performance-threaded_worst  ${GLOG_LIB} ${PLATFORM_LINK_LIBRIES})
     endif(UNIX)

     # GLOG on Linux is easy - but for Windows trickier,. and it doesn't work (as of yet)
     if(WIN32)
       MESSAGE("******************************************************")
       MESSAGE("*** SORRY- Google glog on windows is not preconfigured")
       MESSAGE("*** You have to do this yourself: ref CMakeLists.txt")
       MESSAGE("******************************************************")
       MESSAGE("")
     endif(WIN32)
   endif (USE_GOOGLE_GLOG_PERFORMANCE)


   # 4. create the unit tests for g2log --- ONLY TESTED THE UNIT TEST ON LINUX
   # =========================
   if (USE_G2LOG_UNIT_TEST)
     MESSAGE(" g2log unit testing option ON")
      set(GTEST_DIR ../3rdParty/gtest/gtest-1.7.0)
      set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR} ${GTEST_DIR}/src)
      include_directories(${GTEST_INCLUDE_DIRECTORIES})
      add_library(gtest_170_lib ${GTEST_DIR}/src/gtest-all.cc)
      set_target_properties(gtest_170_lib  PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_RTTI=0")
      enable_testing(true)

     # obs see this: http://stackoverflow.com/questions/9589192/how-do-i-change-the-number-of-template-arguments-supported-by-msvcs-stdtupl
     # and this: http://stackoverflow.com/questions/2257464/google-test-and-visual-studio-2010-rc
  

      SET(tests_to_run test_filechange test_io test_configuration test_concept_sink test_sink)
      SET(helper ${DIR_UNIT_TEST}/testing_helpers.h ${DIR_UNIT_TEST}/testing_helpers.cpp)
      include_directories(${DIR_UNIT_TEST})

      FOREACH(test ${tests_to_run} )
        SET(all_tests  ${all_tests} ${DIR_UNIT_TEST}/${test}.cpp )
         IF(${test} STREQUAL "test_filechange")
           add_executable(${test} ${DIR_UNIT_TEST}/${test}.cpp ${helper})

         #ELSEIF(${test} STREQUAL "test_sink")
         #  add_executable(${test} ${DIR_UNIT_TEST}/${test}.cpp ${helper})
         #  MESSAGE("*****************************************************")

         ELSE()
           add_executable(${test} ../test_main/test_main.cpp ${DIR_UNIT_TEST}/${test}.cpp ${helper})
         ENDIF(${test} STREQUAL "test_filechange")

        set_target_properties(${test} PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_TR1_TUPLE=0")
        set_target_properties(${test} PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_RTTI=0")
        IF( NOT(MSVC))
           set_target_properties(${test} PROPERTIES COMPILE_FLAGS "-isystem -pthread ")
        ENDIF( NOT(MSVC)) 
        target_link_libraries(${test} lib_g3logger gtest_170_lib ${PLATFORM_LINK_LIBRIES})
      ENDFOREACH(test)

 



    

# ... in progress
#      add_executable(test_ALL  ${all_tests})
#      set_target_properties(g2log-unit_test_all PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_TR1_TUPLE=0")
#      set_target_properties(g2log-unit_test_all PROPERTIES COMPILE_DEFINITIONS "GTEST_HAS_RTTI=0")
#     target_link_libraries(${test_ALL}   lib_g3logger gtest_170_lib ${PLATFORM_LINK_LIBRIES})

endif (USE_G2LOG_UNIT_TEST)






