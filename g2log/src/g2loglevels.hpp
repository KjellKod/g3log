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
  const std::string text;
};
const LEVELS DEBUG = {0, "DEBUG"}, INFO = {1, "INFO"}, WARNING = {2, "WARNING"}, FATAL = {3, "FATAL"};

namespace g2 {
namespace internal {
  const LEVELS CONTRACT = {4, "CONTRACT"}, FATAL_SIGNAL{5, "FATAL_SIGNAL"}; 
  
  bool wasFatal(const LEVELS& level);
}

#ifdef G2_DYNAMIC_LOGGING
  // Enable/Disable a log level {DEBUG,INFO,WARNING,FATAL}
  // Obviously: 'enabled_status' set to 'false' - means to disable that log level
  // WARNING: This should be used AFTER \ref g2::initializeLogging
  void setLogLevel(LEVELS level, bool enabled_status);
#endif
  bool logLevel(LEVELS level);

} // g2


#endif // G2_LOG_LEVELS_HPP_
