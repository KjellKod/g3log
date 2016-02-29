# g3log is a KjellKod Logger
# 2015 @author Kjell Hedström, hedstrom@kjellkod.cc
# ==================================================================
# 2015 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own
#    risk and comes  with no warranties.
#
# This code is yours to share, use and modify with no strings attached
#   and no restrictions or obligations.
# ===================================================================



SET(LOG_SRC ${g3log_SOURCE_DIR}/src)
include_directories(${LOG_SRC})
SET(ACTIVE_CPP0xx_DIR "Release")

#cmake -DCMAKE_CXX_COMPILER=clang++ ..
  # WARNING: If Clang for Linux does not work with full C++11 support it might be your
  # installation that is faulty. When I tested Clang on Ubuntu I followed the following
  # description
  #  1) http://kjellkod.wordpress.com/2013/09/23/experimental-g3log-with-clang/
  #  2) https://github.com/maidsafe/MaidSafe/wiki/Hacking-with-Clang-llvm-abi-and-llvm-libc
IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
   MESSAGE("")
   MESSAGE("cmake for Clang ")
   SET(CMAKE_CXX_FLAGS "-Wall -std=c++11 -Wunused -D_GLIBCXX_USE_NANOSLEEP")
   IF (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
       SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++ -pthread")
   ELSE()
       SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
   ENDIF()
   IF (${CMAKE_SYSTEM} MATCHES "FreeBSD-([0-9]*)\\.(.*)")
       IF (${CMAKE_MATCH_1} GREATER 9)
           set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
           set(PLATFORM_LINK_LIBRIES execinfo)
       ENDIF()
   ELSEIF (APPLE)
       set(PLATFORM_LINK_LIBRIES c++abi)
   ELSEIF (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
       set(PLATFORM_LINK_LIBRIES rt c++abi)
   ELSE()
       set(PLATFORM_LINK_LIBRIES rt)
   ENDIF()



ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   MESSAGE("cmake for GCC ")
   IF (APPLE)
       set(CMAKE_CXX_FLAGS "-Wall -Wunused -std=c++11  -pthread -D_GLIBCXX_USE_NANOSLEEP")
   ELSEIF (MINGW)
       set(CMAKE_CXX_FLAGS "-Wall -Wunused -std=c++11  -pthread -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD")
   ELSE()
       set(PLATFORM_LINK_LIBRIES rt)
       set(CMAKE_CXX_FLAGS "-Wall -rdynamic -Wunused -std=c++11 -pthread -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD")
   ENDIF()
ENDIF()


IF (MSVC OR MINGW)
  set(PLATFORM_LINK_LIBRIES dbghelp)
      # VC11 bug: http://code.google.com/p/googletest/issues/detail?id=408
      #          add_definition(-D_VARIADIC_MAX=10)
      # https://github.com/anhstudios/swganh/pull/186/files
      ADD_DEFINITIONS (/D_VARIADIC_MAX=10)
      MESSAGE(STATUS "- MSVC: Set variadic max to 10 for MSVC compatibility")
      # Remember to set set target properties if using GTEST similar to done below on target "unit_test"
      # "set_target_properties(unit_test  PROPERTIES COMPILE_DEFINITIONS "GTEST_USE_OWN_TR1_TUPLE=0")
   MESSAGE("")
   MESSAGE("Windows: Run cmake with the appropriate Visual Studio generator")
   MESSAGE("The generator is one number below the official version number. I.e. VS2013 -> Generator 'Visual Studio 12'")
   MESSAGE("I.e. if VS2013: Please run the command [cmake -DCMAKE_BUILD_TYPE=Release -G \"Visual Studio 12\" ..]")
   MESSAGE("if cmake finishes OK, do 'msbuild g3log.sln /p:Configuration=Release'")
   MESSAGE("then run 'Release\\g3log-FATAL-*' examples")
   MESSAGE("")
ENDIF()

   # GENERIC STEPS
   file(GLOB SRC_FILES ${LOG_SRC}/g3log/*.h ${LOG_SRC}/g3log/*.hpp ${LOG_SRC}/*.cpp ${LOG_SRC}/*.ipp)
   file(GLOB HEADER_FILES ${LOG_SRC}/g3log/*.hpp ${LOG_SRC}/*.hpp)
   #MESSAGE(" HEADER FILES ARE: ${HEADER_FILES}")

   IF (MSVC OR MINGW)
         list(REMOVE_ITEM SRC_FILES  ${LOG_SRC}/crashhandler_unix.cpp)
   ELSE()
         list(REMOVE_ITEM SRC_FILES  ${LOG_SRC}/crashhandler_windows.cpp ${LOG_SRC}/g3log/stacktrace_windows.hpp ${LOG_SRC}/stacktrace_windows.cpp)
   ENDIF (MSVC OR MINGW)

   set(SRC_FILES ${SRC_FILES} ${SRC_PLATFORM_SPECIFIC})

   # Create the g3log library
   include_directories(${LOG_SRC})
   #MESSAGE("  g3logger files: [${SRC_FILES}]")
   add_library(g3logger ${SRC_FILES})
   set_target_properties(g3logger PROPERTIES
      LINKER_LANGUAGE CXX
      OUTPUT_NAME g3logger
      CLEAN_DIRECT_OUTPUT 1)
   target_link_libraries(g3logger ${PLATFORM_LINK_LIBRIES})
   SET(G3LOG_LIBRARY g3logger)

if(ADD_BUILD_WIN_SHARED OR NOT(MSVC OR MINGW))
   IF(NOT(CMAKE_VERSION LESS 3.4) AND MSVC)
      set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
   ENDIF()
   add_library(g3logger_shared SHARED ${SRC_FILES})
   set_target_properties(g3logger_shared PROPERTIES
      LINKER_LANGUAGE CXX
      OUTPUT_NAME g3logger
      CLEAN_DIRECT_OUTPUT 1)
   IF(APPLE)
      set_target_properties(g3logger_shared PROPERTIES MACOSX_RPATH TRUE)
   ENDIF(APPLE)
   target_link_libraries(g3logger_shared ${PLATFORM_LINK_LIBRIES})

   SET(G3LOG_SHARED_LIBRARY g3logger_shared)
endif()



