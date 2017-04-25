# ==========================================================================
# 2010 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
# with no warranties. This code is yours to share, use and modify with no
# strings attached and no restrictions or obligations.
#
# For more information see g3log/LICENSE or refer refer to http://unlicense.org
# ============================================================================*/


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
#     (cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 12")
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

cmake_minimum_required (VERSION 3.1)
ENABLE_LANGUAGE(CXX)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type, one of: Release, Debug" FORCE)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

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


SET(MAJOR_VERSION 1)
IF (${VERSION}.x STREQUAL ".x")
   MESSAGE("Extracting git software version")
   execute_process(COMMAND bash "-c" "git rev-list --branches HEAD | wc -l | tr -d ' ' | tr -d '\n'" OUTPUT_VARIABLE GIT_VERSION)
   SET(MINOR_VERSION 3)
   math(EXPR VERSION-BASE ${GIT_VERSION}/255)
   math(EXPR VERSION-REMAINDER ${GIT_VERSION}%255)
   message("git build version: ${GIT_VERSION}") 
   message("version base: ${VERSION-BASE}") 
   message("version remainder: ${VERSION-REMAINDER}") 
   # SET(BUILD_NUMBER ${GIT_VERSION})
   SET(BUILD_NUMBER ${VERSION-BASE}) 
   SET(VERSION ${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_NUMBER}-${VERSION-REMAINDER}) 
ENDIF()
MESSAGE("Software Version: ${VERSION}")



   # ============================================================================
   # G3LOG OPTIONAL FEATURES
   # ============================================================================
   INCLUDE (${g3log_SOURCE_DIR}/Options.cmake)

   # =========================================================================
   # G3 Macro definitions in Options.cmake are written to file
   #    this avoids having to re-state your definitions in your source code
   #    or compile options
   #==========================================================================
   INCLUDE (${g3log_SOURCE_DIR}/GenerateMacroDefinitionsFile.cmake)



   # =========================================================================
   # G3LOG BUILD
   #==========================================================================
   INCLUDE (${g3log_SOURCE_DIR}/Build.cmake)



   # ============================================================================
   # EXAMPLE OPTIONS: By defauls is ON. This will create 'g3log-FATAL-* examples'
   # ============================================================================
   # DISABLE WITH:  -DADD_FATAL_EXAMPLE=OFF
   INCLUDE (${g3log_SOURCE_DIR}/example/Example.cmake)



   # ============================================================================
   # PERFORMANCE TEST OPTIONS: Performance operations for g3log
   # ============================================================================
   # ENABLE WITH:  -DADD_G3LOG_PERFORMANCE=ON
   INCLUDE (${g3log_SOURCE_DIR}/test_performance/Performance.cmake)



   # ==========================================================================
   # UNIT TEST OPTIONS:
   # ============================================================================
   # ENABLE WITH:  -DADD_G3LOG_UNIT_TEST=ON
   INCLUDE (${g3log_SOURCE_DIR}/test_unit/Test.cmake)


   

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # ==========================================================================
   # BETA : package manager for G3Log,. not yet reliable. Use at your own risk
   #        only tested on Ubuntu and CentOS
   #
   #        For OSX you can install an older version using 'brew install'
   #
   # ==========================================================================
   #   Package handling is done AFTER all other CMake setup
   #   usage:   make package
   #   Check the output result and install accordingly.
   # ==========================================================================
   INCLUDE (${g3log_SOURCE_DIR}/CPackLists.txt)
ENDIF()


IF (NOT MSVC)
   MESSAGE("\n\n
      *******************************************************************
      Please do 'make clean-cmake' before next cmake generation.
         It is a good idea to purge your build directory of CMake
         generated cache files
      *******************************************************************
       ")
   add_custom_target(clean-cmake
      COMMAND ${CMAKE_COMMAND} -P ${g3log_SOURCE_DIR}/CleanAll.cmake
   )
ENDIF()





