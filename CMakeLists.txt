# g3log is a KjellKod Logger
# 2011 @author Kjell Hedström, hedstrom@kjellkod.cc 
# ==================================================================
# 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own
#    risk and comes  with no warranties.
#
# This code is yours to share, use and modify with no strings attached
#   and no restrictions or obligations.
# ===================================================================

# Below are details for compiling on Windows and Linux
# by default only an example g3log binary is created
# the performance and unit tests creation can be enabled by switching their
# OPTIONs from OFF to ON --- See below at around line 110

# === WINDOWS === 
# Example for: Visual Studio 2013 (earlier should work too)
# 1. please use the "Visual Studio Command Prompt 12 (2013)"
# 2. from the g3log folder
#    mkdir build
#    cd build;
# 3. cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio XXX" ..
#    (XXX is the Visual Studio version you are running)
# 4. msbuild g3log.sln /p:Configuration=Release
#
# Try to run an example, such as:
# 5. Release\g3log-FATAL-contract.exe
#
#

# ===   LINUX: === To try this out from folder g3log:
#    mkdir build
#    cd build
# >> create makefiles in g3log/build directory
#   cmake -DCMAKE_BUILD_TYPE=Release .. 
#    make -jN   (where N stands for number of cores you want to utilize)     
#
#
#
# ===  Clang on Linux ===
#   From g3log
#   mkdir build && cd build
#   cmake -DCMAKE_CXX_COMPILER=clang++ ..
#   if you want to double-check settings:   "VERBOSE=1 make" 
#   otherwise just run:   "make -j"                        
#                
# ============================================================================

cmake_minimum_required (VERSION 2.8)
ENABLE_LANGUAGE(CXX)
set(CMAKE_BUILD_TYPE Release)

project (g3log)



# Detect 64 or 32 bit
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
   # 64-bit project
   SET(64_BIT_OS TRUE)
     MESSAGE("A 64-bit OS detected")
 else()
   SET(64_BIT_OS FALSE)
   MESSAGE("A 32-bit OS detected")
endif()



   # =========================================================================
   # G3LOG BUILD
   #==========================================================================

   INCLUDE (${g3log_SOURCE_DIR}/Build.cmake)




   # ============================================================================
   # G3LOG DYNAMIC LEVELS OPTIONS
   # ============================================================================
   # ENABLE WITH:     -USE_DYNAMIC_LOGGING_LEVELS=ON   : run-type turn on/off levels
   INCLUDE (${g3log_SOURCE_DIR}/Dynamic.cmake)



   # ============================================================================
   # EXAMPLE OPTIONS: By defauls is ON. This will create 'g3log-FATAL-* examples'
   # ============================================================================
   # DISABLE WITH:  -DUSE_SIMPLE_EXAMPLE=OFF    
   INCLUDE (${g3log_SOURCE_DIR}/example/Example.cmake)



   # ============================================================================
   # PERFORMANCE TEST OPTIONS: Performance operations for g3log
   # ============================================================================
   # ENABLE WITH:  -USE_G3LOG_PERFORMANCE=ON  
   INCLUDE (${g3log_SOURCE_DIR}/test_performance/Performance.cmake)

      

   # ==========================================================================
   # UNIT TEST OPTIONS: 
   # ============================================================================
   # ENABLE WITH:  -DUSE_G2LOG_UNIT_TEST=ON  
   INCLUDE (${g3log_SOURCE_DIR}/test_unit/Test.cmake)






# ==========================================================================
# BETA : package manager for G3Log,. not yet reliable. Use at your own risk 
# ==========================================================================
#   Package handling is done AFTER all other CMake setup   
#
#   usage:   make package   
#         
#   Check the output result and install accordingly.
# ==========================================================================
# INCLUDE (${g3log_SOURCE_DIR}/CPackLists.txt)






