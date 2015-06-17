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



# -DENABLE_FATAL_SIGNALHANDLING=ON   : defualt change the
# By default fatal signal handling is enabled. You can disable it with this option
# enumerated in src/stacktrace_windows.cpp 
option (ENABLE_FATAL_SIGNALHANDLING
    "Vectored exception / crash handling with improved stack trace" ON)

IF(NOT ENABLE_FATAL_SIGNALHANDLING)
  add_definitions(-DDISABLE_FATAL_SIGNALHANDLING)
    MESSAGE("-DENABLE_FATAL_SIGNALHANDLING=OFF\t\t\tFatal signal handler is disabled. Make sure to have \n\t\t[#define DISABLE_FATAL_SIGNALHANDLING 1] in your source code") 
  ELSE() 
  MESSAGE("-DENABLE_FATAL_SIGNALHANDLING=ON\t\t\tFatal signal handler is enabled")
ENDIF(NOT ENABLE_FATAL_SIGNALHANDLING)


# WINDOWS OPTIONS
IF (MSVC OR MINGW) 
# -DENABLE_VECTORED_EXCEPTIONHANDLING=ON   : defualt change the
# By default vectored exception handling is enabled, you can disable it with this option. 
# Please know that only known fatal exceptions will be caught, these exceptions are the ones
# enumerated in src/stacktrace_windows.cpp 
   option (ENABLE_VECTORED_EXCEPTIONHANDLING
       "Vectored exception / crash handling with improved stack trace" ON)

    IF(NOT ENABLE_VECTORED_EXCEPTIONHANDLING)
      add_definitions(-DDISABLE_VECTORED_EXCEPTIONHANDLING)
      MESSAGE("-DENABLE_VECTORED_EXCEPTIONHANDLING=OFF\t\t\tVectored exception handling is disabled. Make sure to have \n\t\t[#define DISABLE_VECTORED_EXCEPTIONHANDLING 1] in your source code") 

    ELSE() 
       MESSAGE("-DENABLE_VECTORED_EXCEPTIONHANDLING=ON\t\t\tVectored exception handling is enabled") 
    ENDIF(NOT ENABLE_VECTORED_EXCEPTIONHANDLING)




# Default ON. Will trigger a break point in DEBUG builds if the signal handler 
#  receives a fatal signal.
#
   option (DEBUG_BREAK_AT_FATAL_SIGNAL
       "Enable Visual Studio break point when receiving a fatal exception. In __DEBUG mode only" ON)
    IF(DEBUG_BREAK_AT_FATAL_SIGNAL)
      add_definitions(-DDEBUG_BREAK_AT_FATAL_SIGNAL)
      MESSAGE("-DDEBUG_BREAK_AT_FATAL_SIGNAL=ON\t\t\tBreak point for fatal signal is enabled for __DEBUG.Make sure to have \n\t\t[#define DEBUG_BREAK_AT_FATAL_SIGNAL 1] in your source code") 
    ELSE() 
       MESSAGE("-DDEBUG_BREAK_AT_FATAL_SIGNAL=OFF\t\t\tBreak point for fatal signal is disabled") 
    ENDIF(DEBUG_BREAK_AT_FATAL_SIGNAL)

ENDIF (MSVC OR MINGW)


