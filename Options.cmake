# g3log is a KjellKod Logger
# 2015 @author Kjell Hedström, hedstrom@kjellkod.cc 
# ==================================================================
# 2015 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own
#    risk and comes  with no warranties.
#
# This code is yours to share, use and modify with no strings attached
#   and no restrictions or obligations.
# ===================================================================


# PLEASE NOTE THAT:
# the following definitions can through options be added 
# to the auto generated file src/g3log/generated_definitions.hpp
#   add_definitions(-DG3_DYNAMIC_LOGGING)
#   add_definitions(-DCHANGE_G3LOG_DEBUG_TO_DBUG)
#   add_definitions(-DDISABLE_FATAL_SIGNALHANDLING)
#   add_definitions(-DDISABLE_VECTORED_EXCEPTIONHANDLING)
#   add_definitions(-DDEBUG_BREAK_AT_FATAL_SIGNAL)



# Used for generating a macro definitions file  that is to be included
# that way you do not have to re-state the Options.cmake definitions when 
# compiling your binary (if done in a separate build step from the g3log library)
SET(G3_DEFINITIONS "")


# -DUSE_DYNAMIC_LOGGING_LEVELS=ON   : run-type turn on/off levels
option (USE_DYNAMIC_LOGGING_LEVELS
       "Turn ON/OFF log levels. An disabled level will not push logs of that level to the sink. By default dynamic logging is disabled" OFF)
IF(USE_DYNAMIC_LOGGING_LEVELS)
   LIST(APPEND G3_DEFINITIONS G3_DYNAMIC_LOGGING)
   MESSAGE("-DUSE_DYNAMIC_LOGGING_LEVELS=ON")
   MESSAGE("\tDynamic logging levels is used")
   MESSAGE("\tUse  [g3::setLogLevel(LEVEL boolean)] to enable/disable logging on specified levels\n\n")
ELSE() 
  MESSAGE("-DUSE_DYNAMIC_LOGGING_LEVELS=OFF") 
ENDIF(USE_DYNAMIC_LOGGING_LEVELS)




# -DCHANGE_G3LOG_DEBUG_TO_DBUG=ON   : change the DEBUG logging level to be DBUG to avoid clash with other libraries that might have
# predefined DEBUG for their own purposes
option (CHANGE_G3LOG_DEBUG_TO_DBUG
       "Use DBUG logging level instead of DEBUG. By default DEBUG is the debugging level" OFF)
IF(CHANGE_G3LOG_DEBUG_TO_DBUG)
   LIST(APPEND G3_DEFINITIONS CHANGE_G3LOG_DEBUG_TO_DBUG)
   MESSAGE("-DCHANGE_G3LOG_DEBUG_TO_DBUG=ON                 DBUG instead of DEBUG logging level is used")
   ELSE() 
  MESSAGE("-DCHANGE_G3LOG_DEBUG_TO_DBUG=OFF \t(Debuggin logging level is 'DEBUG')") 
ENDIF(CHANGE_G3LOG_DEBUG_TO_DBUG)



# -DENABLE_FATAL_SIGNALHANDLING=ON   : defualt change the
# By default fatal signal handling is enabled. You can disable it with this option
# enumerated in src/stacktrace_windows.cpp 
option (ENABLE_FATAL_SIGNALHANDLING
    "Vectored exception / crash handling with improved stack trace" ON)

IF(NOT ENABLE_FATAL_SIGNALHANDLING)
  LIST(APPEND G3_DEFINITIONS DISABLE_FATAL_SIGNALHANDLING)

    MESSAGE("-DENABLE_FATAL_SIGNALHANDLING=OFF               Fatal signal handler is disabled")
  ELSE() 
  MESSAGE("-DENABLE_FATAL_SIGNALHANDLING=ON\tFatal signal handler is enabled")
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
      LIST(APPEND G3_DEFINITIONS DISABLE_VECTORED_EXCEPTIONHANDLING)
      MESSAGE("-DENABLE_VECTORED_EXCEPTIONHANDLING=OFF           Vectored exception handling is disabled") 
    ELSE() 
       MESSAGE("-DENABLE_VECTORED_EXCEPTIONHANDLING=ON\t\t\tVectored exception handling is enabled") 
    ENDIF(NOT ENABLE_VECTORED_EXCEPTIONHANDLING)




# Default ON. Will trigger a break point in DEBUG builds if the signal handler 
#  receives a fatal signal.
#
   option (DEBUG_BREAK_AT_FATAL_SIGNAL
       "Enable Visual Studio break point when receiving a fatal exception. In __DEBUG mode only" OFF)
    IF(DEBUG_BREAK_AT_FATAL_SIGNAL)
      LIST(APPEND G3_DEFINITIONS DEBUG_BREAK_AT_FATAL_SIGNAL)
      MESSAGE("-DDEBUG_BREAK_AT_FATAL_SIGNAL=ON                  Break point for fatal signal is enabled for __DEBUG.") 
    ELSE() 
       MESSAGE("-DDEBUG_BREAK_AT_FATAL_SIGNAL=OFF\t\t\tBreak point for fatal signal is disabled") 
    ENDIF(DEBUG_BREAK_AT_FATAL_SIGNAL)

ENDIF (MSVC OR MINGW)
MESSAGE("\n\n\n")






