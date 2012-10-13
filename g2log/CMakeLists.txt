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

# 2012-05-29: justthread is no longer necessary on Windows (vs2011) and
#             linux (gcc4.7.1)


# WINDOWS == README: Example how to setup environment + running an example
# Below written for VS11 (2012)
# 1. please use the "Visual Studio Command Prompt 11 (2012)"
# 2. from the g2log folder
#    mkdir build
#    cd build;
# 3. cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 11" ..
#              the "Visual Studio 11" .. does not require just::thread!
# 4. msbuild g2log_by_kjellkod.sln /p:Configuration=Release
# 5. Release\g2log-FATAL-example.exe
#
#
# . LINUX:To try this out from folder g2log:
#    mkdir build
#    cd build
#    cmake ..    # create makefiles in g2log/build directory
#    make    # link active_object, g2log and example code to get an "example" executable
#     ./g2log-FATAL-example
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


IF(UNIX)
   MESSAGE("")
   MESSAGE("cmake for *NIX ")
   MESSAGE("if cmake finishes OK, do make")
   MESSAGE("then run './g2log-FATAL-example' or whatever performance test you feel like trying")
   MESSAGE("")
   set(PLATFORM_LINK_LIBRIES rt)
   # justthread not needed from gcc4.7.1
   #           set(PLATFORM_LINK_LIBRIES justthread rt)

   # -rdynamic is needed for correct stack dumps with demangling
   # -D_GLIBCXX_USE_NANOSLEEP is needed for this_thread sleep (unit testing)
   set(CMAKE_CXX_FLAGS "-Wall -rdynamic -Wunused -std=c++11 ${CMAKE_CXX_FLAGS_DEBUG} -pthread -D_GLIBCXX_USE_NANOSLEEP")
   set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_unix.cpp)

   # -- justthread not needed for gcc4.7.1
   #     set(CMAKE_CXX_FLAGS "-Wall -rdynamic -Wunused -std=c++0x ${CMAKE_CXX_FLAGS_DEBUG} -pthread -I/usr/include/justthread")
   # -- justhtread not needed for gcc4.7.1
   #               include_directories("/usr/include/justthread")
   ENDIF(UNIX)

   if (MSVC)
      # VC11 bug: http://code.google.com/p/googletest/issues/detail?id=408
      #          add_definition(-D_VARIADIC_MAX=10)
          # https://github.com/anhstudios/swganh/pull/186/files
      ADD_DEFINITIONS (/D_VARIADIC_MAX=10)
      MESSAGE(STATUS "- MSVC: Set variadic max to 10 for MSVC compatibility")
          # Remember to set set target properties if using GTEST similar to done below on target "unit_test"
          # "set_target_properties(unit_test  PROPERTIES COMPILE_DEFINITIONS "GTEST_USE_OWN_TR1_TUPLE=0")
   endif ()


#Visual Studio 2010 -- must use justthread. For now hardcoded for x64
IF(MSVC10)
   MESSAGE("")
   MESSAGE("Windows: Please run the command [cmake -DCMAKE_BUILD_TYPE=Release -G \"Visual Studio 10\" ..]")
   MESSAGE("if cmake finishes OK, do 'msbuild g2log_by_kjellkod.sln /p:Configuration=Release'")
   MESSAGE("then run 'Release\\g2log-FATAL-example.exe'  or whatever performance test you feel like trying")
   MESSAGE("")
   set(PLATFORM_LINK_LIBRIES $ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/lib/justthread_vc10_mdd.lib)
   #set(PLATFORM_LINK_LIBRIES $ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/lib/justthread_vc10x64_mdd.lib)
   set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_win.cpp)
   include_directories("$ENV{PROGRAMFILES}/JustSoftwareSolutions/JustThread/include")
ENDIF(MSVC10)

# Visual Studio 2011 -- std::thread etc are included with the Visual Studio package, so justthread dependencies are removed
IF(MSVC11)
   MESSAGE("")
   MESSAGE("Windows: Please run the command [cmake -DCMAKE_BUILD_TYPE=Release -G \"Visual Studio 11\" ..]")
   MESSAGE("if cmake finishes OK, do 'msbuild g2log_by_kjellkod.sln /p:Configuration=Release'")
   MESSAGE("then run 'Release\\g2log-FATAL-example.exe'  or whatever performance test you feel like trying")
   MESSAGE("")
   set(SRC_PLATFORM_SPECIFIC ${LOG_SRC}/crashhandler_win.cpp)
ENDIF(MSVC11)


   # GENERIC STEPS
   # CODE SOURCES these +
   set(SRC_CPP ${LOG_SRC}/g2logworker.cpp ${LOG_SRC}/g2log.cpp ${LOG_SRC}/g2time.cpp)
   set(SRC_H ${LOG_SRC}/g2logworker.h ${LOG_SRC}/g2log.h ${LOG_SRC}/crashhandler.h ${LOG_SRC}/g2time.h ${LOG_SRC}/g2future.h)
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
   # ---- by default all is OFF: except 'g2log-FATAL-example -----
   # ---- the reason for this is that
   # ----- 1) the performance tests were only thoroughly tested on Ubuntu, not windows-
   #           (g2log windows/linux, but Google's glog only on linux)
   #
   #       2) The unit test were tested windows/linux,. but must be unzipped
   #          before it can be "cmake'd" and compiled --- leaving it as OFF for now
   # ============================================================================
   # 1. a simple test example 'g2log-FATAL-example'
   option (USE_SIMPLE_EXAMPLE
          "Create  simple binaries that runs a few LOG calls" ON)


   # 2. performance test (average + worst case) for KjellKod's g2log
   option (USE_G2LOG_PERFORMANCE
         "Create performance tests for g2log" OFF)

   # 3. performance test for Google's glog
   option (USE_GOOGLE_GLOG_PERFORMANCE
         "Create performance tests for Google's glog (remember to install glog, snapshot available at g2log/3rdParty/glog)" OFF)

   # 4. unit test for g2log
   option (USE_G2LOG_UNIT_TEST
          "Create unit test for g2log (remember to unzip Google's gtest framework, available at g2log/3rdParty/gtest)" ON)




   # ===============================================================================================
   #
   # BELOW : Creating executables depending on OPTIONS above
   #
   #  ==============================================================================================
   # 1. create the the example EXECUTABLE - hook in the test_example's CMakeLists.txt file
   if (USE_SIMPLE_EXAMPLE)
      MESSAGE(" g2log-FATAL-example option ON")
      include_directories (${DIR_EXAMPLE})
      add_executable(g2log-FATAL-example ${DIR_EXAMPLE}/main.cpp)
      target_link_libraries(g2log-FATAL-example lib_activeobject lib_g2logger ${PLATFORM_LINK_LIBRIES})
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
   # ===============================================================================================


   # 4. create the unit tests for g2log --- ONLY TESTED THE UNIT TEST ON LINUX
   if (USE_G2LOG_UNIT_TEST)
     MESSAGE(" g2log unit testing option ON")
    # SETUP for GTEST
      set(GTEST_DIR ../3rdParty/gtest/gtest-1.6.0__stripped)
      set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR} ${GTEST_DIR}/src)
      include_directories(${GTEST_INCLUDE_DIRECTORIES})
      add_library(gtest_160_lib ${GTEST_DIR}/src/gtest-all.cc ${GTEST_DIR}/src/gtest_main.cc)
      enable_testing(true)

      add_executable(g2log-unit_test ../test_main/test_main.cpp ${DIR_UNIT_TEST}/test_io.cpp ${DIR_UNIT_TEST}/test_configuration.cpp)
      # obs see this: http://stackoverflow.com/questions/9589192/how-do-i-change-the-number-of-template-arguments-supported-by-msvcs-stdtupl
      set_target_properties(g2log-unit_test  PROPERTIES COMPILE_DEFINITIONS "_VARIADIC_MAX=10")
      # and this: http://stackoverflow.com/questions/2257464/google-test-and-visual-studio-2010-rc
      set_target_properties(g2log-unit_test  PROPERTIES COMPILE_DEFINITIONS "GTEST_USE_OWN_TR1_TUPLE=0")
      target_link_libraries(g2log-unit_test lib_activeobject lib_g2logger gtest_160_lib ${PLATFORM_LINK_LIBRIES})


      add_executable(g2log-unit_test_filechange ${DIR_UNIT_TEST}/test_filechange.cpp)
      set_target_properties(g2log-unit_test_filechange  PROPERTIES COMPILE_DEFINITIONS "_VARIADIC_MAX=10")
      set_target_properties(g2log-unit_test_filechange  PROPERTIES COMPILE_DEFINITIONS "GTEST_USE_OWN_TR1_TUPLE=0")
          target_link_libraries(g2log-unit_test_filechange lib_activeobject lib_g2logger gtest_160_lib ${PLATFORM_LINK_LIBRIES})

          endif (USE_G2LOG_UNIT_TEST)






