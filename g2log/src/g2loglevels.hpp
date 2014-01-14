/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Filename:g2loglevels.hpp  Part of Framework for Logging and Design By Contract
* Created: 2012 by Kjell Hedström
*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#ifndef G2_LOG_LEVELS_HPP_
#define G2_LOG_LEVELS_HPP_

#include <string>

// Levels for logging, made so that it would be easy to change, remove, add levels -- KjellKod
struct LEVELS
{
  const int value;
  const char* text;
};

#if _MSC_VER
// Visual Studio does not support constexpr
// what is below is ugly and not right but it works in this purpose.
// if there are any issues with it then just please remove all instances of constexpr
  #ifndef constexpr
     #define constexpr 
  #endif
#endif
constexpr const LEVELS DEBUG{0, "DEBUG"}, 
             INFO{DEBUG.value+1, "INFO"}, 
             WARNING{INFO.value+1, "WARNING"}, 
             // Insert here *any* extra logging levels that is needed
             // 1) Remember to update the FATAL initialization below
             // 2) Remember to update the initialization of "g2loglevels.cpp/g_log_level_status"
             FATAL{WARNING.value+1, "FATAL"};

namespace g2 {
namespace internal {
  constexpr const LEVELS CONTRACT{100, "CONTRACT"}, FATAL_SIGNAL{101, "FATAL_SIGNAL"}; 
  
  bool wasFatal(const LEVELS& level);
}

#ifdef G2_DYNAMIC_LOGGING
  // Enable/Disable a log level {DEBUG,INFO,WARNING,FATAL}
  void setLogLevel(LEVELS level, bool enabled_status);
#endif
  bool logLevel(LEVELS level);

} // g2


#endif // G2_LOG_LEVELS_HPP_
