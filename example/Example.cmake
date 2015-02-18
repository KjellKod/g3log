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
   option (USE_FATAL_EXAMPLE  "Fatal (fatal-crashes/contract) examples " ON)


   IF (USE_FATAL_EXAMPLE)

      MESSAGE("-DUSE_FATAL_EXAMPLE=ON\t\t\tg3log-FATAL- [contract][sigsegv][choice] are examples of when g3log comes in handy")
      include_directories (${DIR_EXAMPLE})
      add_executable(g3log-FATAL-contract ${DIR_EXAMPLE}/main_contract.cpp)
      add_executable(g3log-FATAL-sigsegv ${DIR_EXAMPLE}/main_sigsegv.cpp)
      add_executable(g3log-FATAL-choice ${DIR_EXAMPLE}/main_fatal_choice.cpp)
      
      target_link_libraries(g3log-FATAL-contract ${G3LOG_LIBRARY})
      target_link_libraries(g3log-FATAL-sigsegv ${G3LOG_LIBRARY})
      target_link_libraries(g3log-FATAL-choice ${G3LOG_LIBRARY})
   ELSE()
       MESSAGE("-DUSE_SIMPLE_EXAMPLE=OFF")
   ENDIF (USE_FATAL_EXAMPLE)