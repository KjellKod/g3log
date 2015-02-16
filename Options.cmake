# -DUSE_DYNAMIC_LOGGING_LEVELS=ON   : run-type turn on/off levels
option (USE_DYNAMIC_LOGGING_LEVELS
       "Turn ON/OFF log levels. An disabled level will not push logs of that level to the sink. By default dynamic logging is disabled" OFF)
IF(USE_DYNAMIC_LOGGING_LEVELS)
   add_definitions(-DG2_DYNAMIC_LOGGING)
   MESSAGE("-DUSE_DYNAMIC_LOGGING_LEVELS=ON")
   MESSAGE("\tDynamic logging levels can be turned on. Make sure to have \n\t\t[#define G2_DYNAMIC_LOGGING 1] in your source code")
   MESSAGE("\t\tUse  [g2::setLogLevel(LEVEL boolean)] to enable/disable logging on specified levels")
ELSE() 
  MESSAGE("-DUSE_DYNAMIC_LOGGING_LEVELS=OFF") 
ENDIF(USE_DYNAMIC_LOGGING_LEVELS)




# -DCHANGE_G3LOG_DEBUG_TO_DBUG=ON   : change the DEBUG logging level to be DBUG to avoid clash with other libraries that might have
# predefined DEBUG for their own purposes
option (CHANGE_G3LOG_DEBUG_TO_DBUG
       "Use DBUG logging level instead of DEBUG. By default DEBUG is the debugging level" OFF)
IF(CHANGE_G3LOG_DEBUG_TO_DBUG)
   add_definitions(-DCHANGE_G3LOG_DEBUG_TO_DBUG)
   MESSAGE("-DCHANGE_G3LOG_DEBUG_TO_DBUG=ON")
   MESSAGE("\tDBUG instead of DEBUG logging level can be used. Make sure to have \n\t\t[#define CHANGE_G3LOG_DEBUG_TO_DBUG 1] in your source code")
ELSE() 
  MESSAGE("-DCHANGE_G3LOG_DEBUG_TO_DBUG=OFF")
  MESSAGE("\tDebuggin logging level is 'DEBUG'") 
ENDIF(CHANGE_G3LOG_DEBUG_TO_DBUG)



# WINDOWS OPTION
# -DENABLE_VECTORED_EXCEPTIONHANDLING=ON   : defualt change the
# By default vectored exception handling is enabled, you can disable it with this option. 
# Please know that only known fatal exceptions will be caught, these exceptions are the ones
# enumerated in src/stacktrace_windows.cpp 
IF (MSVC OR MINGW) 
   option (ENABLE_VECTORED_EXCEPTIONHANDLING
       "Vectored exception / crash handling with improved stack trace" ON)

   IF(NOT ENABLE_VECTORED_EXCEPTIONHANDLING)
      add_definitions(-DDISABLE_VECTORED_EXCEPTIONHANDLING)
      MESSAGE("-DENABLE_VECTORED_EXCEPTIONHANDLING=OFF") 
      MESSAGE("\tVectored exception handling is disabled")
    ELSE() 
       MESSAGE("-DENABLE_VECTORED_EXCEPTIONHANDLING=ON")
       MESSAGE("\tVectored exception handling is enabled") 
    ENDIF(NOT ENABLE_VECTORED_EXCEPTIONHANDLING)
ENDIF (MSVC OR MINGW)