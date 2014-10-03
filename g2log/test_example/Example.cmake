   # ==============================================================
   #   -DUSE_SIMPLE_EXAMPLE=OFF   : to turn off the fatal examples
   #
   #    
   #  Leaving it to ON will create 
   #                        g3log-FATAL-sigsegv
   #                        g3log-FATAL-contract
   #
   # ==============================================================


   set(DIR_EXAMPLE ${g3log_SOURCE_DIR}/example)
   option (USE_SIMPLE_EXAMPLE  "Simple (fatal-crash/contract) examples " ON)


   IF (USE_SIMPLE_EXAMPLE)

      MESSAGE("-DUSE_SIMPLE_EXAMPLE=ON")
      MESSAGE("\tg3log-FATAL-contract and g3log-FATAL-sigsegv shows fatal examples of when g3log comes in handy")
      include_directories (${DIR_EXAMPLE})
      add_executable(g3log-FATAL-contract ${DIR_EXAMPLE}/main_contract.cpp)
      add_executable(g3log-FATAL-sigsegv ${DIR_EXAMPLE}/main_sigsegv.cpp)
      target_link_libraries(g3log-FATAL-contract ${G3LOG_LIBRARY} ${PLATFORM_LINK_LIBRIES})
      target_link_libraries(g3log-FATAL-sigsegv ${G3LOG_LIBRARY}  ${PLATFORM_LINK_LIBRIES})
   ELSE()
       MESSAGE("-DUSE_SIMPLE_EXAMPLE=OFF")
   ENDIF (USE_SIMPLE_EXAMPLE)