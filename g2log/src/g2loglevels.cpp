/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* ============================================================================
* Filename:g2loglevels.cpp  Part of Framework for Logging and Design By Contract
* Created: 2012 by Kjell Hedström
*
* PUBLIC DOMAIN and Not copywrited. First published at KjellKod.cc
* ********************************************* */

#include "g2loglevels.hpp"
#include "g2log.hpp"
#include <atomic>
#include <cassert>
namespace g2
{
  namespace internal {
     bool wasFatal(const LEVELS& level) { 
        return level.value >= FATAL.value; 
     }

     // All levels are by default ON: i.e. for DEBUG, INFO, WARNING, FATAL
    constexpr const int size{ FATAL.value + 1 };
    std::atomic<bool> g_log_level_status[4]{{true}, {true}, {true},{true}};   
	
	#ifndef _MSC_VER
	static_assert(4 == size, "Mismatch between number of logging levels and their use");
	#endif
  } // internal


#ifdef G2_DYNAMIC_LOGGING
  void setLogLevel(LEVELS log_level, bool enabled)
  {
    // MSC: remove when constexpr available. see static_assert above
    assert(size == 4 && "Mismatch between number of logging levels and their use"); 
    int level = log_level.value;
    CHECK((level >= DEBUG.value) && (level <= FATAL.value));
    internal::g_log_level_status[level].store(enabled, std::memory_order_release);
  }
#endif

  
  bool logLevel(LEVELS log_level)
  {
#ifdef G2_DYNAMIC_LOGGING
    int level = log_level.value;
    CHECK((level >= DEBUG.value) && (level <= FATAL.value));
    bool status = (internal::g_log_level_status[level].load(std::memory_order_acquire));
    return status;
#endif
    return true;
  }


} // g2
