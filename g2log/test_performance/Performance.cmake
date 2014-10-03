   # 2. performance test (average + worst case) for KjellKod's g2log
   #    Do 'cmake -DUSE_G2LOG_PERFORMANCE=ON' to enable this 
   option (USE_G2LOG_PERFORMANCE "g2log performance test" OFF)




   # 2. create the g2log's performance tests
   # =========================
   IF (USE_G2LOG_PERFORMANCE)
       set(DIR_PERFORMANCE ${g3log_SOURCE_DIR}/test_performance)

      MESSAGE("-DUSE_G2LOG_PERFORMANCE=ON")
      include_directories (${DIR_PERFORMANCE})

      # MEAN PERFORMANCE TEST
      add_executable(g2log-performance-threaded_mean
                    ${DIR_PERFORMANCE}/main_threaded_mean.cpp 
                    ${DIR_PERFORMANCE}/performance.h)
      # Turn on G2LOG performance flag
      set_target_properties(g2log-performance-threaded_mean PROPERTIES 
                            COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
      target_link_libraries(g2log-performance-threaded_mean 
                             ${G3LOG_LIBRARY}  ${PLATFORM_LINK_LIBRIES})

     # WORST CASE PERFORMANCE TEST
     add_executable(g2log-performance-threaded_worst 
                    ${DIR_PERFORMANCE}/main_threaded_worst.cpp ${DIR_PERFORMANCE}/performance.h)
     # Turn on G2LOG performance flag
     set_target_properties(g2log-performance-threaded_worst  PROPERTIES 
                           COMPILE_DEFINITIONS "G2LOG_PERFORMANCE=1")
     target_link_libraries(g2log-performance-threaded_worst  
                            ${G3LOG_LIBRARY}  ${PLATFORM_LINK_LIBRIES})

   ELSE()
      MESSAGE("-DUSE_G2LOG_PERFORMANCE=OFF")
   ENDIF(USE_G2LOG_PERFORMANCE)



