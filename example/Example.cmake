# g3log is a KjellKod Logger
# 2015 @author Kjell Hedström, hedstrom@kjellkod.cc 
# ==================================================================
# 2015 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own
#    risk and comes  with no warranties.
#
# This code is yours to share, use and modify with no strings attached
#   and no restrictions or obligations.
# ===================================================================



   

# ==============================================================
   #   -DUSE_SIMPLE_EXAMPLE=OFF   : to turn off the fatal examples
   #
   #    
   #  Leaving it to ON will create 
   #                        g3log-FATAL-sigsegv
   #                        g3log-FATAL-contract
   #
   # ==============================================================

  IF (MSVC OR MINGW)
     set(EXAMPLE_PLATFORM_LINK_LIBRIES dbghelp)
  ENDIF()

   set(DIR_EXAMPLE ${g3log_SOURCE_DIR}/example)
   option (ADD_FATAL_EXAMPLE  "Fatal (fatal-crashes/contract) examples " ON)


   IF (ADD_FATAL_EXAMPLE)
      message( STATUS "-DADD_FATAL_EXAMPLE=ON" )
      message( STATUS "\t\t[contract][sigsegv][fatal choice] are examples of when g3log comes in handy\n" )
      include_directories (${DIR_EXAMPLE})
      add_executable(g3log-FATAL-contract ${DIR_EXAMPLE}/main_contract.cpp)
      add_executable(g3log-FATAL-sigsegv ${DIR_EXAMPLE}/main_sigsegv.cpp)
      add_executable(g3log-FATAL-choice ${DIR_EXAMPLE}/main_fatal_choice.cpp)
      
      target_link_libraries(g3log-FATAL-contract ${G3LOG_LIBRARY} ${EXAMPLE_PLATFORM_LINK_LIBRIES})
      target_link_libraries(g3log-FATAL-sigsegv ${G3LOG_LIBRARY} ${EXAMPLE_PLATFORM_LINK_LIBRIES})
      target_link_libraries(g3log-FATAL-choice ${G3LOG_LIBRARY} ${EXAMPLE_PLATFORM_LINK_LIBRIES})
   ELSE()
       message( STATUS "-DADD_SIMPLE_EXAMPLE=OFF" )
   ENDIF (ADD_FATAL_EXAMPLE)
