# g3log is a KjellKod Logger
# 2015 @author Kjell Hedström, hedstrom@kjellkod.cc 
# ==================================================================
# 2015 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own
#    risk and comes  with no warranties.
#
# This code is yours to share, use and modify with no strings attached
#   and no restrictions or obligations.
# ===================================================================




   # . performance test (average + worst case) for KjellKod's g3log
   #    Do 'cmake -DADD_G3LOG_BENCH_PERFORMANCE=ON' to enable this 
   option (ADD_G3LOG_BENCH_PERFORMANCE "g3log performance test" OFF)




   #  create the g3log's performance tests
   # =========================
   IF (ADD_G3LOG_BENCH_PERFORMANCE)
       set(DIR_PERFORMANCE ${g3log_SOURCE_DIR}/test_performance)
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
       set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

      message( STATUS "-DADD_G3LOG_BENCH_PERFORMANCE=ON" )
      include_directories (${DIR_PERFORMANCE})

      find_library(TCMALLOC_LIBRARY NAMES tcmalloc)
      if(NOT TCMALLOC_LIBRARY)
        message(FATAL_ERROR "tcmalloc library not found")
      else()
         message( STATUS "tcmalloc: ${TCMALLOC_LIBRARY} was found" )
      endif()
      
      find_library(PROFILER NAMES profiler)
      if(NOT PROFILER)
        message(FATAL_ERROR "PROFILER library not found")
      else()
         message( STATUS "PROFILER: ${PROFILER} was found" )
      endif()


      # add_executable(your_executable ${YOUR_SOURCES})
      # target_link_libraries(your_executable PRIVATE ${TCMALLOC_LIBRARY})

      

      # MEAN PERFORMANCE TEST
      add_executable(g3log-performance-threaded_mean
                    ${DIR_PERFORMANCE}/main_threaded_mean.cpp 
                    ${DIR_PERFORMANCE}/performance.h)
      # Turn on G3LOG performance flag
      set_target_properties(g3log-performance-threaded_mean PROPERTIES 
                            COMPILE_DEFINITIONS "G3LOG_PERFORMANCE=1")
      target_link_libraries(g3log-performance-threaded_mean 
                             ${G3LOG_LIBRARY}  ${PLATFORM_LINK_LIBRIES} ${PROFILER} )

     # WORST CASE PERFORMANCE TEST
     add_executable(g3log-performance-threaded_worst 
                    ${DIR_PERFORMANCE}/main_threaded_worst.cpp ${DIR_PERFORMANCE}/performance.h)
     # Turn on G3LOG performance flag
     set_target_properties(g3log-performance-threaded_worst  PROPERTIES 
                           COMPILE_DEFINITIONS "G3LOG_PERFORMANCE=1")
     target_link_libraries(g3log-performance-threaded_worst  
                            ${G3LOG_LIBRARY}  ${PLATFORM_LINK_LIBRIES} ${PROFILER})

   ELSE()
      message( STATUS "-DADD_G3LOG_BENCH_PERFORMANCE=OFF" )
   ENDIF(ADD_G3LOG_BENCH_PERFORMANCE)



