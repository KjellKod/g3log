   # 2. performance test (average + worst case) for KjellKod's g3log
   #    Do 'cmake -DUSE_G3LOG_PERFORMANCE=ON' to enable this 
   option (USE_G3LOG_PERFORMANCE "g3log performance test" OFF)




   # 2. create the g3log's performance tests
   # =========================
   IF (USE_G3LOG_PERFORMANCE)
       set(DIR_PERFORMANCE ${g3log_SOURCE_DIR}/test_performance)

      MESSAGE("-DUSE_G3LOG_PERFORMANCE=ON")
      include_directories (${DIR_PERFORMANCE})

      # MEAN PERFORMANCE TEST
      add_executable(g3log-performance-threaded_mean
                    ${DIR_PERFORMANCE}/main_threaded_mean.cpp 
                    ${DIR_PERFORMANCE}/performance.h)
      # Turn on G3LOG performance flag
      set_target_properties(g3log-performance-threaded_mean PROPERTIES 
                            COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
      target_link_libraries(g3log-performance-threaded_mean 
                             ${G3LOG_LIBRARY}  ${PLATFORM_LINK_LIBRIES})

     # WORST CASE PERFORMANCE TEST
     add_executable(g3log-performance-threaded_worst 
                    ${DIR_PERFORMANCE}/main_threaded_worst.cpp ${DIR_PERFORMANCE}/performance.h)
     # Turn on G3LOG performance flag
     set_target_properties(g3log-performance-threaded_worst  PROPERTIES 
                           COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
     target_link_libraries(g3log-performance-threaded_worst  
                            ${G3LOG_LIBRARY}  ${PLATFORM_LINK_LIBRIES})

   ELSE()
      MESSAGE("-DUSE_G3LOG_PERFORMANCE=OFF")
   ENDIF(USE_G3LOG_PERFORMANCE)



