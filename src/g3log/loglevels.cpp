/** ==========================================================================
* 2012 by KjellKod.cc. This is PUBLIC DOMAIN to use at your own risk and comes
* with no warranties. This code is yours to share, use and modify with no
* strings attached and no restrictions or obligations.
* 
* For more information see g3log/LICENSE or refer refer to http://unlicense.org
* ============================================================================*/

#include "g3log/loglevels.hpp"
#include "g3log/g3log.hpp"
#include <atomic>
#include <cassert>

namespace g2 {
   namespace internal {
      bool wasFatal(const LEVELS &level) {
         return level.value >= FATAL.value;
      }

#ifdef G2_DYNAMIC_LOGGING
      // All levels are by default ON: i.e. for DEBUG, INFO, WARNING, FATAL
      const int g_level_size {
         FATAL.value + 1
      };
      std::atomic<bool> g_log_level_status[4] {{true}, {true}, {true}, {true}};
#endif
   } // internal

#ifdef G2_DYNAMIC_LOGGING
   void setLogLevel(LEVELS log_level, bool enabled) {
      assert(internal::g_level_size == 4 && "Mismatch between number of logging levels and their use");
      int level = log_level.value;
      CHECK((level >= g2::kDebugVaulue) && (level <= FATAL.value));
      internal::g_log_level_status[level].store(enabled, std::memory_order_release);
   }
#endif

   bool logLevel(LEVELS log_level) {
#ifdef G2_DYNAMIC_LOGGING
      int level = log_level.value;
      CHECK((level >= g2::kDebugVaulue) && (level <= FATAL.value));
      bool status = (internal::g_log_level_status[level].load(std::memory_order_acquire));
      return status;
#endif
      return true;
   }
} // g2
