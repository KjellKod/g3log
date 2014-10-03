SET(LOG_SRC ${g3log_SOURCE_DIR}/src)
include_directories(${LOG_SRC})
SET(ACTIVE_CPP0xx_DIR "Release")

#cmake -DCMAKE_CXX_COMPILER=clang++ ..
  # WARNING: If Clang for Linux does not work with full C++11 support it might be your
  # installation that is faulty. When I tested Clang on Ubuntu I followed the following
  # description 
  #  1) http://kjellkod.wordpress.com/2013/09/23/experimental-g2log-with-clang/
  #  2) https://github.com/maidsafe/MaidSafe/wiki/Hacking-with-Clang-llvm-abi-and-llvm-libc
IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
   MESSAGE("")
   MESSAGE("cmake for Clang ")
   IF (APPLE)
       set(PLATFORM_LINK_LIBRIES c++abi)
   ELSE()
       set(PLATFORM_LINK_LIBRIES rt  c++abi)
   ENDIF()
  SET(CMAKE_CXX_FLAGS  "-Wall -std=c++11  -pthread -stdlib=libc++ -Wunused -D_GLIBCXX_USE_NANOSLEEP")



ELSEIF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   MESSAGE("cmake for GCC ")
   IF (APPLE)
       set(CMAKE_CXX_FLAGS "-Wall -Wunused -std=c++11  -pthread -D_GLIBCXX_USE_NANOSLEEP")
   ELSE()
       set(PLATFORM_LINK_LIBRIES rt)
       set(CMAKE_CXX_FLAGS "-Wall -rdynamic -Wunused -std=c++11 -pthread -D_GLIBCXX_USE_NANOSLEEP -D_GLIBCXX_USE_SCHED_YIELD")
   ENDIF()



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
   MESSAGE("if cmake finishes OK, do 'msbuild g3log.sln /p:Configuration=Release'")
   MESSAGE("then run 'Release\\g3log-FATAL-*' examples")
   MESSAGE("")
ENDIF()




   # GENERIC STEPS
   file(GLOB SRC_FILES ${LOG_SRC}/*.h ${LOG_SRC}/*.hpp ${LOG_SRC}/*.cpp ${LOG_SRC}/*.ipp)
   file(GLOB HEADER_FILES ${LOG_SRC}/*.h ${LOG_SRC}/*.hpp)
   #MESSAGE(" HEADER FILES ARE: ${HEADER_FILES}")

   IF (MSVC OR MINGW) 
         list(REMOVE_ITEM SRC_FILES  ${LOG_SRC}/crashhandler_unix.cpp)
   ELSE()     
         list(REMOVE_ITEM SRC_FILES  ${LOG_SRC}/crashhandler_win.cpp)
   ENDIF (MSVC OR MINGW)

   set(SRC_FILES ${SRC_FILES} ${SRC_PLATFORM_SPECIFIC})
 
   # Create the g3log library
   include_directories(${LOG_SRC})
   #MESSAGE("  g3logger files: [${SRC_FILES}]")
   add_library(g3logger ${SRC_FILES})
   set_target_properties(g3logger PROPERTIES LINKER_LANGUAGE CXX)
   add_library(g3logger_shared SHARED ${SRC_FILES})
   set_target_properties(g3logger_shared PROPERTIES LINKER_LANGUAGE CXX)
  
   SET(G3LOG_SHARED_LIBRARY g3logger_shared)
   SET(G3LOG_LIBRARY g3logger)



