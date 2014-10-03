# -DUSE_DYNAMIC_LOGGING_LEVELS=ON   : run-type turn on/off levels
#  
#
option (USE_DYNAMIC_LOGGING_LEVELS
       "Turn ON/OFF log levels. An disabled level will not push logs of that level to the sink. By default dynamic logging is disabled" OFF)



IF(USE_DYNAMIC_LOGGING_LEVELS)
   add_definitions(-DG2_DYNAMIC_LOGGING)
   MESSAGE("-DUSE_DYNAMIC_LOGGING_LEVELS=ON  
             \tDynamic logging levels can be turned on. Make sure to have \n\t\t[#define G2_DYNAMIC_LOGGING 1] in your source code")
   MESSAGE("\t\tUse  [g2::setLogLevel(LEVEL boolean)] to enable/disable logging on specified levels")
ELSE() 
  MESSAGE("-DUSE_DYNAMIC_LOGGING_LEVELS=OFF") 
ENDIF(USE_DYNAMIC_LOGGING_LEVELS)


